/*
 * ui.c
 * 
 * General UI code; basically, anything that has visible effects is
 * implemented here.
 * 
 * Copyright (c) 2008 Paulo Matias
 * Copyright (c) 2004 Matthew Reppert
 * 
 * Use of this code is permitted under the code of the GNU General
 * Public License version 2 (and only version 2).
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
/*#include <X11/xpm.h>*/
#include <xembed.h>

#include "trace.h"
#include "systray.h"
#include "ui.h"
#include "hidden_list.h"
#include "configuration_files.h"


Display *main_disp;
Window main_wind, sel_wind, icon_wind, draw_wind;
char *display_string = NULL;
/*char *geometry_string = "64x64+0+0";*/
int width, height, pos_x, pos_y;
/*Pixmap bg_pixmap;*/
char *bg_data;

/*int wmaker = 1;*/
int iconsize = 14;
int loop_program = 1;
/*static GC main_gc;*/

/* kludge! */
int skip_next_leave_event=0;
int skip_next_enter_event=0;


#if 0
XRectangle active_area = {
	.x = 0, .y = 0, .width = iconsize, .height = iconsize
};

static XRectangle left_button_area = {
	.x = 0, .y = 0, .width = 0, .height = 0
};

static XRectangle right_button_area = {
	.x = 16, .y = 0, .width = 0, .height = 0
};
#endif


/*
 * wmsystray_resize
 *
 * This will resize the systray main window and set its SizeHints.
 */
void wmsystray_resize(int width, int height) {
	XSizeHints *size_hints;
	
	/* Unmap windows to be sure the Window Manager will be notified. */
	XUnmapWindow (main_disp, icon_wind);
	XUnmapWindow (main_disp, main_wind);
	XFlush (main_disp);
	
	/* Set the size hints and resize this stuff! */
	
	size_hints = XAllocSizeHints();
	
	size_hints->flags = PSize | PMinSize | PMaxSize;
	size_hints->min_width  = size_hints->max_width  = size_hints->width  = width;
 	size_hints->min_height = size_hints->max_height = size_hints->height = height;

	XSetWMNormalHints (main_disp, icon_wind, size_hints);
	XResizeWindow     (main_disp, icon_wind, width, height);
 
	XSetWMNormalHints (main_disp, main_wind, size_hints);
	XResizeWindow     (main_disp, main_wind, width, height);
	
	/* Remap windows so the user can see them. */
	XMapWindow (main_disp, main_wind);
	XMapSubwindows (main_disp, icon_wind);
	XFlush (main_disp);
	
	XFree (size_hints);
}


/*
 * init_ui
 *
 * Create the main window and get it ready for use.
 */
int init_ui(char *app_name, int argc, char **argv) {
	XWMHints *wm_hints;
	/*XSizeHints *size_hints;*/
	XClassHint *class_hints;
	int border_width;
	/*int status;*/

	TRACE((stderr, "ENTERING: init_ui\n"));

	border_width = 0;

	/*
	 * Try to open a connection to an X server.
	 */
	main_disp = XOpenDisplay(display_string);
	if (main_disp == NULL) {
		TRACE((stderr, "Couldn't open display %s!\n",
				display_string ? display_string : ":0.0"));
		return -1;
	}

#if 0
	size_hints = XAllocSizeHints();
	XWMGeometry (main_disp, DefaultScreen(main_disp), geometry_string,
		     NULL, border_width, size_hints,
		     &size_hints->x, &size_hints->y,
		     &size_hints->width, &size_hints->height, &status);
#endif

	/*
	 * Create our windows.
	 */
	main_wind = XCreateSimpleWindow (main_disp,
					 DefaultRootWindow (main_disp),	
					 0, 0,
					 iconsize, iconsize,
					 0, 0, 0);

	icon_wind = XCreateSimpleWindow (main_disp,
					 DefaultRootWindow (main_disp),
					 0, 0,
					 iconsize, iconsize,
					 0, 0, 0);
	/*XFree(size_hints);*/

	draw_wind = icon_wind;
#if 0
	if (!wmaker)
		draw_wind = main_wind;
#endif

	sel_wind = XCreateSimpleWindow (main_disp, DefaultRootWindow(main_disp),
					-1, -1, 1, 1, 0, 0, 0);

	/*
	 * Set window manager hints.
	 */
	class_hints = XAllocClassHint();
	class_hints->res_class = "WMSYSTRAY";
	class_hints->res_name = app_name;
	XSetClassHint (main_disp, main_wind, class_hints);
	XFree (class_hints);

	/*
	 * The icon window and SetCommand hints are necessary for icon
	 * window and dockapp stuff to work properly with Window Maker.
	 */
	wm_hints = XAllocWMHints();
	wm_hints->flags = WindowGroupHint | IconWindowHint | StateHint;
	wm_hints->window_group = main_wind;
	wm_hints->icon_window = icon_wind;
	wm_hints->initial_state = WithdrawnState;
	XSetWMHints (main_disp, main_wind, wm_hints);
	XFree (wm_hints);

	XSetCommand (main_disp, main_wind, argv, argc);


	/*
	 * Display windows.
	 */
	XMapWindow (main_disp, main_wind);
	XMapSubwindows (main_disp, icon_wind);
	XFlush (main_disp);
	
	/*
	 * Resize the window to its default size.
	 */	
	wmsystray_resize(iconsize, iconsize);

	/*
	 * Set the icon window background.
	 */
#if 0
	status=XpmCreatePixmapFromData (main_disp, DefaultRootWindow(main_disp),
				 wmsystray_xpm, &bg_pixmap, NULL, NULL);
	if (status != XpmSuccess || status != 0) 
		TRACE((stderr, "XPM loading didn't work\n"));

	/* XSetWindowBackgroundPixmap (main_disp, icon_wind, bg_pixmap); */
#endif
	XSetWindowBackgroundPixmap (main_disp, draw_wind, ParentRelative);
	XClearWindow (main_disp, draw_wind);
	XFlush (main_disp);

	/*draw_ui_elements();*/

	/*
	 * Select input events we're interested in.
	 */
	XSelectInput(main_disp, main_wind, VisibilityChangeMask |
					   StructureNotifyMask |
					   ExposureMask |
					   PropertyChangeMask | 
					   /*
					   ButtonPressMask |
					   ButtonReleaseMask |
					   */
		    			   EnterWindowMask|
		     			   LeaveWindowMask|
		                           PointerMotionMask|
					   KeyPressMask |
					   KeyReleaseMask);

	XSelectInput(main_disp, icon_wind, VisibilityChangeMask |
					   StructureNotifyMask |
					   ExposureMask |
					   PropertyChangeMask | 
					   /*
					   ButtonPressMask |
					   ButtonReleaseMask |
					   */
		    			   EnterWindowMask|
		     			   LeaveWindowMask|
		                           PointerMotionMask|
					   KeyPressMask |
					   KeyReleaseMask);


	TRACE((stderr, "LEAVING: init_ui\n"));
	return 0;
}



/*
 * cleanup_ui
 *
 * Clean up the main window and free related resources.
 */
void cleanup_ui() {
	TRACE((stderr, "ENTERING: cleanup_ui\n"));

	XUnmapWindow (main_disp, main_wind);
	XDestroyWindow (main_disp, main_wind);
	XDestroyWindow (main_disp, icon_wind);
	XDestroyWindow (main_disp, sel_wind);
	XCloseDisplay (main_disp);

	TRACE((stderr, "LEAVING: cleanup_ui\n"));
}


#if 0
/*
 * draw_ui_elements
 *
 * Draw the systray area and buttons.
 */
void draw_ui_elements() {
	XCopyArea (main_disp, bg_pixmap, draw_wind,
		   DefaultGC(main_disp, DefaultScreen(main_disp)),
		   active_area.x - 1, active_area.y - 1,
		   active_area.width + 2, active_area.height + 2,
		   active_area.x - 1, active_area.y - 1);
	XCopyArea (main_disp, bg_pixmap, draw_wind,
		   DefaultGC(main_disp, DefaultScreen(main_disp)),
		   left_button_area.x, left_button_area.y,
		   left_button_area.width, left_button_area.height,
		   left_button_area.x - 1, left_button_area.y - 1);
	XCopyArea (main_disp, bg_pixmap, draw_wind,
		   DefaultGC(main_disp, DefaultScreen(main_disp)),
		   right_button_area.x, right_button_area.y,
		   right_button_area.width, right_button_area.height,
		   right_button_area.x - 1, right_button_area.y - 1);
	XFlush (main_disp);
}
#endif


/*
 * point_is_in_rect
 *
 * Determines whether a point is in a rectangle.
 */
int point_is_in_rect (int x, int y, XRectangle *rect) {
	int ret = 0;
	int x2, y2;

	x2 = rect->x + rect->width;
	y2 = rect->y + rect->height;
	if (x >= rect->x && y >= rect->y && x <= x2 && y <= y2)
		ret = 1;

	return ret;
}


void print_item_info(const char* fname)
{
	struct list_head *n;
	struct systray_item *item;
	XClassHint class_hint;
	FILE *fptr;

	fptr = fopen(fname, "w");

	list_for_each (n, &systray_list) {
		item = list_entry (n, struct systray_item, systray_list);
		XGetClassHint(main_disp, item->window_id, &class_hint);
		fprintf(fptr, "%s\n", class_hint.res_name);
		XFree(class_hint.res_name);
		XFree(class_hint.res_class);
	}
	fflush(fptr);
	fclose(fptr);
}


/*
 * wmsystray_handle_signal
 *
 * Handle UNIX signals.
 */
void wmsystray_handle_signal (int signum) {
	switch (signum) {
		case SIGUSR1:
			print_item_info("/tmp/trayion-icon-list.txt");
			break;
		case SIGUSR2:
			reload_config_files();
			repaint_systray(0);
			printf("reloading config files.\n");
			break;
		case SIGINT:
		case SIGTERM:
			loop_program = 0;
			break;
	}
}

void handle_enter_event()
{
	if (!skip_next_enter_event){
		show_hidden = 1;
		skip_next_leave_event = 1;
		repaint_systray(0);
	} else {
		skip_next_enter_event = 0;
	}
}
void handle_leave_event()
{
	if (!skip_next_leave_event){
		show_hidden=0;
		repaint_systray(0);
	} else {
		skip_next_leave_event = 0;
		skip_next_enter_event = 1;
	}
}

void check_pointer_inside_tray_kludge()
{
	Window root_return, child_return;
	int pointer_root_x, pointer_root_y, window_x, window_y;
	unsigned int pointer_mask;
	static int iteration=0;
	XWindowAttributes attrib;
	iteration++;
	if (iteration%100==0) {
		XQueryPointer(main_disp, icon_wind, &root_return, &child_return, 
			      &pointer_root_x, &pointer_root_y,
			      &window_x, &window_y, &pointer_mask);
		XGetWindowAttributes(main_disp, main_wind, &attrib);
		if(!child_return && show_hidden && !skip_next_leave_event){
			show_hidden = 0;
			XSync(main_disp, False);
			repaint_systray(0);
		}
	}
}
/*
 * wmsystray_event_loop
 *
 * Handle X events.
 */
void wmsystray_event_loop() {
	XEvent ev;
	XWindowAttributes attrib;
#if 0
	Window cover;
	struct list_head *n;
#endif
	struct systray_item *item;

	while (loop_program) {
		while (XPending(main_disp)) {
			XNextEvent(main_disp, &ev);
			/* TRACE((stderr, "XEVENT %x\n", ev.type)); */

			/*
			 * There are several XEMBED events that clients may send
			 * to us.
			 */
			if (xembed_event_is_xembed_event(main_disp, &ev)) {
				switch (ev.xclient.data.l[1]) {
				case XEMBED_REQUEST_FOCUS:
					xembed_focus_in(main_disp,
							ev.xclient.window,
							XEMBED_FOCUS_CURRENT);
					break;

				case XEMBED_FOCUS_NEXT:
					break;

				case XEMBED_FOCUS_PREV:
					break;

				case XEMBED_REGISTER_ACCELERATOR:
					break;

				case XEMBED_UNREGISTER_ACCELERATOR:
					break;
				}

				continue;
			}

			if (event_is_systray_event(&ev)) {
				handle_systray_event (&ev);
				continue;
			}

			switch (ev.type) {
			case MapRequest:
				item = find_systray_item(ev.xmaprequest.window);
				if (item) {
					TRACE((stderr, "MRAISING: %X\n",
						(unsigned int)item->window_id));
					XMapRaised (main_disp, item->window_id);
					xembed_window_activate (main_disp,
							item->window_id);
							/*
					xembed_focus_in (main_disp,
							item->window_id, 1);
							*/
				}
				break;
			case Expose:
				/*draw_ui_elements();*/
				break;

			case PropertyNotify:
				if (ev.xproperty.window == main_wind)
					break;

				TRACE((stderr, "Prop %x\n", (unsigned int)ev.xproperty.window));
				item = find_systray_item (ev.xproperty.window);
				if (item && systray_property_update (item) == 0)
					break;

				break;

			case ConfigureNotify:
				if (ev.xany.window == main_wind ||
				    ev.xany.window == icon_wind) 
					break;
					

				/*
				 * Don't let icons resize themselves to other
				 * than iconsize x iconsize.
				 */
				XGetWindowAttributes (main_disp,
							ev.xproperty.window,
							&attrib);
							
				TRACE((stderr, "Window %x trying to resize to %dx%d.\n", (unsigned int)ev.xproperty.window, attrib.width, attrib.height));

				if ((attrib.height != iconsize)){
					int ww;
					ww = scale_item_width(attrib.width, attrib.height, iconsize);
					XResizeWindow (main_disp,
						       ev.xproperty.window,
						       ww, iconsize);
					
					wmsystray_resize(systray_total_width(), 14);
					repaint_systray(0);
				}	

				/*
				cover = XCreateSimpleWindow (main_disp,
								draw_wind,
								0, 0,
								64, 64,
								0, 0, 0);
				XMapWindow (main_disp, cover);
				XDestroyWindow (main_disp, cover);
				*/
				break;

			case ReparentNotify:
			case UnmapNotify:
			case DestroyNotify:
				if (ev.xreparent.parent == main_wind ||
				    ev.xreparent.parent == icon_wind)
					break;

				item = find_systray_item (ev.xreparent.window);
				if (item) {
					TRACE ((stderr, "\tRemove %x\n",
						(unsigned int)item->window_id));
					systray_item_count--;
					list_del (&item->systray_list);
					repaint_systray(0);
				}

				break;

			case KeyPress:
			case KeyRelease:
				/* systray_forward_event(&ev); */
				break;

			case FocusIn:
				/* systray_focus_in(&ev); */
				break;

			case FocusOut:
				/* systray_focus_out(&ev); */
				break;
			case EnterNotify:
				handle_enter_event();
				break;
			case LeaveNotify:
				handle_leave_event();
				break;
			case MotionNotify:
				break;
			}
		}
		usleep(100000L);
		check_pointer_inside_tray_kludge();
	}
}
