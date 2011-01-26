/*
 * xembed.c
 * 
 * XEmbed protocol implementation.
 * 
 * Copyright (c) 2008 Paulo Matias
 * Copyright (c) 2004 Matthew Reppert
 * 
 * Use of this code is permitted under the code of the GNU General
 * Public License version 2 (and only version 2).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "xembed.h"
#include "../trayion/trace.h"


static int xembed_trapped_error;
static int (*xembed_old_error_handler) (Display *, XErrorEvent *);



/*
 * xembed_error_handler
 */
static int xembed_error_handler (Display *disp, XErrorEvent *err) {
	xembed_trapped_error = err->error_code;
	return 0;
}



/*
 * xembed_event_is_xembed_event
 */
int xembed_event_is_xembed_event (Display *disp, XEvent *ev) {
	int ret = 0;

	if (ev->xclient.type == ClientMessage &&
	    ev->xclient.message_type == XInternAtom (disp, "_XEMBED", False) &&
	    ev->xclient.format == 32)
	{
		ret = 1;
	}

	return ret;
}



/*
 * xembed_forward_event
 */
int xembed_forward_event (Display *disp, Window client, XEvent *ev) {
	ev->xkey.window = client;
	xembed_trapped_error = 0;
	xembed_old_error_handler = XSetErrorHandler(&xembed_error_handler);
	XSendEvent (disp, client, False, NoEventMask, ev);
	XSync (disp, False);
	XSetErrorHandler (xembed_old_error_handler);

	return xembed_trapped_error;
}



/*
 * xembed_send_message
 *
 * Send an XEMBED message to a window.
 */
int xembed_send_message (Display *disp, Window to_wind,
			  long message, long d1, long d2, long d3)
{
	XEvent ev;

	TRACE((stderr, "\tSending XEMBED %ld (%x, %x, %x) to %x\n",
		message, (unsigned int)d1, (unsigned int)d2, (unsigned int)d3, (unsigned int)to_wind));
	memset (&ev, 0, sizeof(ev));
	ev.xclient.type = ClientMessage;
	ev.xclient.window = to_wind;
	ev.xclient.message_type = XInternAtom (disp, "_XEMBED", False);
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = CurrentTime;
	ev.xclient.data.l[1] = message;
	ev.xclient.data.l[2] = d1;
	ev.xclient.data.l[3] = d2;
	ev.xclient.data.l[4] = d3;

	xembed_trapped_error = 0;
	xembed_old_error_handler = XSetErrorHandler(&xembed_error_handler);
	XSendEvent (disp, to_wind, False, NoEventMask, &ev);
	XSync (disp, False);

	XSetErrorHandler(xembed_old_error_handler);
	if (xembed_trapped_error)
		/* Do something? */;

	return xembed_trapped_error;
}



/*
 * xembed_get_info
 *
 * Get the XEMBED info for a window.
 */
int xembed_get_info (Display *disp, Window wind, struct xembed_info *info) {
	Atom embed_info_atom, type;
	int format, status;
	unsigned long nitems, bytes_after;
	/*unsigned long einfo[2];*/
	/*unsigned long version, flags;*/
	unsigned char *data;
	unsigned long *data_long;

	TRACE((stderr, "ENTERING: xembed_get_info\n"));

	/* XXX Error handler wrap */
	embed_info_atom = XInternAtom (disp, "_XEMBED_INFO", False);
	status = XGetWindowProperty (disp, wind, embed_info_atom,
				     0, 2, False,
				     embed_info_atom, &type, &format,
				     &nitems, &bytes_after, &data);
	XSync (disp, False);

	if (status != Success) {
		return -1;
	}

	if (type == None) {
		return -1;
	}

	if (type != embed_info_atom) {
		return -1;
	}

	if (nitems < 2) {
		return -1;
	}

	if (!data) {
		return -1;
	}

	data_long = (unsigned long *) data;
	info->version = data_long[0]; 
	info->flags = data_long[1] & XEMBED_INFO_FLAGS_ALL;

	TRACE((stderr, "LEAVING: xembed_get_info %x %x\n",
		(unsigned int)info->version, (unsigned int)info->flags));

	return 0;
}



/*
 * xembed_unembed_window
 *
 * Have the embedder end XEMBED protocol communication with a child.
 */
int xembed_unembed_window (Display *disp, Window child) {
	xembed_trapped_error = 0;
	xembed_old_error_handler = XSetErrorHandler(&xembed_error_handler);

	XUnmapWindow (disp, child);
	XReparentWindow (disp, child, DefaultRootWindow(disp), 0, 0);
	XSync (disp, False);

	XSetErrorHandler(xembed_old_error_handler);
	if (xembed_trapped_error)
		/* Do something? */;

	return xembed_trapped_error;
}



/*
 * xembed_embedded_notify
 *
 * Notify a window that its embed request has been received and accepted.
 */ 
int xembed_embedded_notify (Display *disp, Window client, Window embedder,
			    long version)
{
	return xembed_send_message (disp, client, XEMBED_EMBEDDED_NOTIFY,
				    0, embedder, version);
}



/*
 * xembed_window_activate
 *
 * Notify a window that it has become active.
 */
int xembed_window_activate (Display *disp, Window client) {
	return xembed_send_message (disp, client, XEMBED_WINDOW_ACTIVATE,
				    0, 0, 0);
}



/*
 * xembed_window_deactivate
 *
 * Notify a window that it has become inactive.
 */
int xembed_window_deactivate (Display *disp, Window client) {
	return xembed_send_message (disp, client, XEMBED_WINDOW_DEACTIVATE,
				    0, 0, 0);
}



/*
 * xembed_focus_in
 *
 * Indicate to an embedded window that it has focus.
 */
int xembed_focus_in (Display *disp, Window client, long focus_type) {
	return xembed_send_message (disp, client, XEMBED_FOCUS_IN,
				    focus_type, 0, 0);
}



/*
 * xembed_focus_out
 *
 * Indicate to an embedded window that it has lost focus.
 */
int xembed_focus_out (Display *disp, Window client) {
	return xembed_send_message (disp, client, XEMBED_FOCUS_OUT, 0, 0, 0);
}



/*
 * xembed_focus_next
 *
 * Sent from the client to the embedder when it reaches the end of its logical
 * tab chain after the user tabbed forward.
 */
int xembed_focus_next (Display *disp, Window client) {
	return xembed_send_message (disp, client, XEMBED_FOCUS_NEXT, 0, 0, 0);
}



/*
 * xembed_focus_prev
 *
 * Sent from the client to the embedder when it reaches the beginning of its
 * logical tab chain after the user tabbed backward.
 */
int xembed_focus_prev (Display *disp, Window client) {
	return xembed_send_message (disp, client, XEMBED_FOCUS_PREV, 0, 0, 0);
}


/*
 * xembed_register_accelerator
 *
 * Sent from a client to an embedder when it wishes to reserve a certain
 * key/modifier combination as a shortcut. Embedders will propogate these
 * upwards if they are embedded.
 */
int xembed_register_accelerator (Display *disp, Window client, long accel_id,
				 long keysym, long modifiers)
{
	return xembed_send_message (disp, client, XEMBED_REGISTER_ACCELERATOR,
				    accel_id, keysym, modifiers);
}



/*
 * xembed_unregister_accelerator
 *
 * Sent to an embedder from a client to unregister an accelerator.
 */
int xembed_unregister_accelerator (Display *disp, Window client, long id) {
	return xembed_send_message (disp, client,
				    XEMBED_UNREGISTER_ACCELERATOR, id, 0, 0);
}



/*
 * xembed_activate_accelerator
 *
 * Sent to a client when a registered accelerator is activated on the toplevel
 * containing its embedder.
 */
int xembed_activate_accelerator (Display *disp, Window client, long id,
				 long flags)
{
	return xembed_send_message (disp, client, XEMBED_ACTIVATE_ACCELERATOR,
				    id, flags, 0);
}



/*
 * xembed_modality_on
 *
 * Sent from embedder to client when the window becomes shadowed by a modal
 * dialog.
 */
int xembed_modality_on (Display *disp, Window client) {
	return xembed_send_message (disp, client, XEMBED_MODALITY_ON,
				    0, 0, 0);
}



/*
 * xembed_modality_off
 *
 * Sent from embedder to client when a window is now longer obscured by a
 * modal dialog.
 */
int xembed_modality_off (Display *disp, Window client) {
	return xembed_send_message (disp, client, XEMBED_MODALITY_OFF,
				    0, 0, 0);
}
