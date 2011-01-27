/*
 * systray.h
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

#ifndef WMSYSTRAY_SYSTRAY_H
#define WMSYSTRAY_SYSTRAY_H

#include <xembed.h>
#include "list.h"


struct systray_item {
	Window window_id;
	struct xembed_info info;
	int rank;
	struct list_head systray_list;
};

extern struct list_head systray_list;
extern int systray_item_count;
extern int place_hidden_items_on_the_left;

/*
 * System tray message opcodes. (From the freedesktop.org specification.)
 */
#define SYSTEM_TRAY_REQUEST_DOCK	0  /* Begin icon docking */
#define SYSTEM_TRAY_BEGIN_MESSAGE	1  /* Display balloon mesage */
#define SYSTEM_TRAY_CANCEL_MESSAGE	2  /* Cancel previous balloon message */


int scale_item_width(int preferred_width, int preferred_height, int iconsize);
int systray_total_width();
int init_systray();
void cleanup_systray();
int event_is_systray_event(XEvent *ev);
int handle_systray_event(XEvent *ev);
void repaint_systray(int new_icon);
struct systray_item *find_systray_item(Window id);
int systray_property_update(struct systray_item *item);

#endif
