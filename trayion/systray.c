/*
 * systray.c
 * 
 * Initialize and clean up the systray area; handle X protocol junk
 * relating to systray communication.
 * 
 * Copyright (c) 2008 Paulo Matias
 * Copyright (c) 2004 Matthew Reppert
 * 
 * Use of this code is permitted under the code of the GNU General
 * Public License version 2 (and only version 2).
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <xembed.h>

#include "list.h"
#include "list_sort.h"
#include "systray.h"
#include "trace.h"
#include "ui.h"
#include "sorted_classes.h"
#include "hidden_list.h"


static Atom systray_atom = None;
static Atom opcode_atom = None;
static Atom message_atom = None;
struct list_head systray_list;
struct list_head *current_item = &systray_list;

int systray_item_count = 0, systray_current_item_no = 0;
int place_hidden_items_on_the_left=1;

int handle_dock_request (Window embed_wind);


/*
 * init_systray
 *
 * Initializes the system tray area and registers it with X.
 */
int init_systray() {
	char atom_name[22] = "_NET_SYSTEM_TRAY_S0";
	XEvent ev;

	INIT_LIST_HEAD (&systray_list);

	/*
	 * "On startup, the system tray must acquire a manager selection
	 *  called _NET_SYSTEM_TRAY_Sn, replacing n with the screen number
	 *  the tray wants to use." - freedesktop System Tray protocol
	 */
	snprintf(atom_name, 21, "_NET_SYSTEM_TRAY_S%d", DefaultScreen(main_disp));
	systray_atom = XInternAtom (main_disp, atom_name, False);
	XSetSelectionOwner (main_disp, systray_atom, sel_wind, CurrentTime);
	if (XGetSelectionOwner (main_disp, systray_atom) != sel_wind)
		return -1;

	opcode_atom = XInternAtom (main_disp, "_NET_SYSTEM_TRAY_OPCODE", False);
	message_atom = XInternAtom (main_disp, "_NET_SYSTEM_TRAY_MESSAGE_DATA",
				    False);

	/*
	 * Selection managers are required to broadcast their existence when
	 * they become selection managers.
	 */
	ev.type = ClientMessage;
	ev.xclient.message_type = XInternAtom (main_disp, "MANAGER", False);
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = CurrentTime;
	ev.xclient.data.l[1] = systray_atom; 
	ev.xclient.data.l[2] = sel_wind;
	ev.xclient.data.l[3] = 0;
	ev.xclient.data.l[4] = 0;
	XSendEvent (main_disp, DefaultRootWindow(main_disp), False,
		    StructureNotifyMask, &ev);

	return 0;
}



/*
 * cleanup_systray
 *
 * Deregisters us as the systray, informs all docked clients of our departure,
 * and cleans up any lingering resources.
 */
void cleanup_systray() {
	struct list_head *n;
	struct systray_item *item;

	if (systray_atom != None) 
		XSetSelectionOwner (main_disp, systray_atom, None, CurrentTime);

	list_for_each (n, &systray_list) {
		item = list_entry (n, struct systray_item, systray_list);
		xembed_unembed_window (main_disp, item->window_id);
	}
}



/*
 * event_is_systray_event
 *
 * Returns non-zero if an X event was sent from a systray client.
 */
int event_is_systray_event(XEvent *ev) {
	int ret = 0;

	/*
	 * "Tray icons can send "opcodes" to the system tray. These are X client
	 *  messages, sent with NoEventMask, a message_type of
	 *  _NET_SYSTEM_TRAY_OPCODE, and format 32." - fd.o System Tray protocol
	 */
	if (ev->xclient.type == ClientMessage &&
	    ev->xclient.message_type == opcode_atom &&
	    ev->xclient.format == 32)
	{
		ret = 1;
	}

	return ret;
}



/*
 * systray_property_update
 *
 * Handle a program changing its _XEMBED_INFO properties. Returns zero if
 * _XEMBED_INFO properties changed, nonzero if they didn't.
 */
int systray_property_update (struct systray_item *item) {
	struct xembed_info info;
	int flags_changed;

	TRACE((stderr, "ENTERING: systray_property_update\n"));

	if (xembed_get_info (main_disp, item->window_id, &info) < 0)
		return -1;
	flags_changed = info.flags ^ item->info.flags;
	if (flags_changed == 0)
		return -1;

	item->info.flags = info.flags;
	if (flags_changed & XEMBED_MAPPED) {
		if (info.flags & XEMBED_MAPPED) {
			TRACE2((stderr, "\tMapping %x on xembed info change\n",
				(unsigned int)item->window_id));

			XMapRaised (main_disp, item->window_id);
			xembed_window_activate (main_disp, item->window_id);
		} else {
			TRACE2((stderr,"\tUnmapping %x on xembed info change\n",
				(unsigned int)item->window_id));

			XUnmapWindow (main_disp, item->window_id);
			xembed_window_deactivate (main_disp, item->window_id);
			xembed_focus_out (main_disp, item->window_id);
		}
	}

	TRACE((stderr, "LEAVING: systray_property_update\n"));
	return 0;
}



/*
 * find_systray_item
 *
 * Given a window id, find the associated systray_item; if there is none,
 * return NULL.
 */
struct systray_item *find_systray_item (Window id) {
	struct list_head *n;
	struct systray_item *item;

	item = NULL;

	list_for_each (n, &systray_list) {
		item = list_entry (n, struct systray_item, systray_list);
		if (item->window_id == id)
			return item;
	}

	return NULL;
}



/*
 * handle_systray_event
 *
 * Deal with a systray event. (We assume that the given event has already been
 * verified to be a systray event, e.g. using event_is_systray_event.)
 *
 * The first data field is a timestap (e.g. CurrentTime); the second data
 * field is an integer indicating the opcode (SYSTEM_TRAY_XXX in systray.h).
 */
int handle_systray_event(XEvent *ev) {
	TRACE((stderr, "ENTERING: handle_systray_event\n"));

	TRACE((stderr, "Systray event to %x\n", (unsigned int)ev->xclient.window));
	switch (ev->xclient.data.l[1]) {
	case SYSTEM_TRAY_REQUEST_DOCK:
		/*
		 * xclient.data.l2 contains the icon tray window ID.
		 */
		TRACE2((stderr, "DOCK REQUEST from window %x\n",
				 (unsigned int)ev->xclient.data.l[2]));
		handle_dock_request (ev->xclient.data.l[2]);
		break;

	case SYSTEM_TRAY_BEGIN_MESSAGE:
		/*
		 * "xclient.data.l[2] contains the timeout in thousandths of
		 *  a second or zero for infinite timeout, ...l[3] contains
		 *  the lengeth of the message string in bytes, not including
		 *  any nul bytes, and ...l[4] contains an ID number for the
		 *  message."
		 */

		break;

	case SYSTEM_TRAY_CANCEL_MESSAGE:
		break;
	}

	TRACE((stderr, "LEAVING: handle_systray_event\n"));
	return 0;	
}

/*
 * scale_item_width
 *
 * Returns the scaled width of the item assuming its height is iconsize,
 * keeping the item's suggested aspect ratio
 */
int scale_item_width(int preferred_width, int preferred_height, int iconsize)
{
	return (preferred_width * iconsize) / preferred_height;
}

int systray_total_width()
{
	struct list_head *l;
	int tw=0;
	struct systray_item *item;
	XWindowAttributes xwa;
   
	tw = 0;
	list_for_each (l, &systray_list) {
		item = list_entry (l, struct systray_item, systray_list);
		if (!XGetWindowAttributes(main_disp, item->window_id, &xwa)){
			/* FIXME: do something better than exit! */
			fprintf(stderr, "Some error occurred!\n");
			exit(1);
		}
		if (show_hidden || !is_hidden(item->window_id))
			tw += xwa.width;
	}
	return tw;
}
/*
 * systray_item_at_coords
 *
 * Return the item at the given point.
 * FIXME: it should check the size of each window!
 */
struct systray_item *systray_item_at_coords (int x, int y) {
	int item;
	struct systray_item *ret;
	struct list_head *n;

	/*x -= 8
	y -= 4;*/
	item = x / iconsize;

	n = systray_list.next;
	while (item > 0) {
		n = n->next;
		item--;

		if (n == &systray_list)
			break;
	}

	if (n == &systray_list)
		ret = NULL;
	else
		ret = list_entry (n, struct systray_item, systray_list);

	return ret;
}

void recalc_window_ranks()
{
	struct list_head *l;
	struct systray_item *item;
   
	list_for_each (l, &systray_list) {
		item = list_entry (l, struct systray_item, systray_list);
		item->rank = window_rank(item->window_id);
		if (is_hidden(item->window_id))
			item->rank = - item->rank;
	}
}

int systray_list_length()
{
	struct list_head *n;
	int length = 0;
	list_for_each (n, &systray_list) {
		length++;
	}
	return length;
}
int systray_list_is_sorted()
{
	struct systray_item *item;
	struct list_head *n;
	int prev_rank;
	item = list_entry (systray_list.next, struct systray_item, systray_list);
	prev_rank = item->rank;
	list_for_each (n, &systray_list) {
		item = list_entry (n, struct systray_item, systray_list);
		if(item->rank < prev_rank)
			return 0;
		prev_rank = item->rank;
	}
	return 1;
}
int compare_items(struct list_head *a, struct list_head *b)
{
	struct systray_item *it1, *it2;
	it1 = list_entry(a, struct systray_item, systray_list);
	it2 = list_entry(b, struct systray_item, systray_list);
	return it1->rank - it2->rank;
}
void sort_systray_list()
{
	list_sort(&systray_list, compare_items);
}

/*
 * repaint_systray
 *
 * Repaint our systray area.
 */
void repaint_systray(int new_icon) {
	struct systray_item *item;
	struct list_head *n;
	int x, y, w;
	int i = 0;
	XWindowAttributes wa;
	TRACE((stderr, "ENTERING: repaint_systray\n"));

	/*draw_ui_elements();*/
	x = 0;
	if(!systray_list_is_sorted()){
		printf("resorting\n");
		sort_systray_list();
	}
	list_for_each (n, &systray_list) {
		
		item = list_entry (n, struct systray_item, systray_list);

		y = 0;
		XGetWindowAttributes(main_disp, item->window_id, &wa);
		/* KLUDGE: it seems like all the newly mapped icons
		   are suggesting an aspect ratio of 2:1, which
		   isn't desirable.  Therefore newly mapped
		   icons are resized to iconsize x iconsize.
		   Resize requests are handled correctly, and
		   the width is kept once is set
		*/
		w =  (new_icon == item->window_id) ? iconsize :  scale_item_width(wa.width, wa.height, iconsize);
		XResizeWindow(main_disp, item->window_id, w, iconsize);
		if (show_hidden || !is_hidden(item->window_id)){
			XMoveWindow (main_disp, item->window_id, x, y);
			x+=w;
			i++;
		}else {
			/* Move window outside the clipping area */
			XMoveWindow(main_disp, item->window_id, -w, -2 * iconsize);
		}

#if 0
		if (i == 4)
			break;
#endif
	}
	TRACE((stderr, "\n"));
	
	/* Resize the area. */
	TRACE((stderr, "RESIZE: resizing systray to %d icons.\n", systray_item_count));
	if(systray_item_count > 0)
		wmsystray_resize(systray_total_width(), iconsize);
	else
		wmsystray_resize(iconsize, iconsize);
	
	XSync (main_disp, False);
	TRACE((stderr, "LEAVING: repaint_systray\n"));
}



/*
 * print_geometry
 *
 * Get a Window's geometry and print it.
 */
void print_geometry (Window embed_wind) {
	int x, y;
	unsigned int width, height, border_width, depth;
	Window parent;

	XGetGeometry (main_disp, embed_wind, &parent, &x, &y,
			&width, &height, &border_width, &depth);
	TRACE((stderr, "\tEmbedded %x has parent %x (%x)\n",
			(unsigned int)embed_wind, (unsigned int)parent, (unsigned int)main_wind));
	TRACE((stderr, "\t%dx%d+%d+%d, border %d, depth %d\n",
			width, height, x, y, border_width, depth));
}

/*
 * handle_dock_request
 *
 * Handle System Tray Protocol dock requests from windows.
 */
int handle_dock_request (Window embed_wind) {
	struct systray_item *item;
	struct xembed_info info;
	int new_x, new_y, status;
	long version;

	TRACE((stderr, "ENTERING: handle_dock_request\n"));
	new_x = new_y = 0;
	version = XEMBED_VERSION;
#if 0
	if (systray_item_count == 4) {
		XReparentWindow (main_disp, embed_wind,
				DefaultRootWindow(main_disp), 0, 0);
		TRACE((stderr, "REJECTED!\n"));
		return -1;
	}
#endif

	XSelectInput (main_disp, embed_wind, StructureNotifyMask |
						PropertyChangeMask);

	XWithdrawWindow (main_disp, embed_wind, 0);
	XReparentWindow (main_disp, embed_wind, draw_wind, 0, 0);
	XSync (main_disp, False);


	/*
	 * Insert the new item into our list of embedded systray clients.
	 */
	item = malloc (sizeof (struct systray_item));
	INIT_LIST_HEAD (& item->systray_list);
	memcpy (&item->info, &info, sizeof(info));
	item->window_id = embed_wind;
	item->rank = window_rank(item->window_id);
	if (place_hidden_items_on_the_left && is_hidden(item->window_id)){
		item->rank = - item->rank;
	}
	list_add_tail (&item->systray_list, &systray_list);

	/*SelectInput was here*/
	status = xembed_get_info (main_disp, embed_wind, &info);
	XSync (main_disp, False);

	if (info.version < XEMBED_VERSION)
		version = info.version;

	TRACE2((stderr, "Using protocol version %ld\n", version));
	status = xembed_embedded_notify (main_disp, embed_wind, draw_wind,
					 version);

	/*
	 * XXX Unconditional mapping works for now, it's probably bad. Anyway,
	 * we need to track ProcessNotify events with the next changeset,
	 * probably.
	 *
	 * Actually, thinking about this coming home, it may be appropriate for
	 * a sys tray to have windows always thinking they're mappend and have
	 * focus.
	 *
	 * Also, unconditional mapping probably works because it's MapRaised,
	 * and thus embedded windows always get events even though we're not
	 * forwarding.
	 */
	if (info.flags & XEMBED_MAPPED) {
		TRACE((stderr, "Mapping\n"));
		XMapRaised (main_disp, embed_wind);
		/*
		status = xembed_window_activate (main_disp, embed_wind);
		status = xembed_focus_in (main_disp, embed_wind, 1);
		*/
	}

	/*
	 * Sync states with the embedded window.
	 */
	systray_item_count++;
	/*
	print_geometry (embed_wind);
	status = xembed_modality_off (main_disp, embed_wind);
	XSync (main_disp, False);
	*/
	/* sort_systray_list(); */
	repaint_systray(item->window_id);
	TRACE((stderr, "LEAVING: handle_dock_request\n"));
	return 0;
}
