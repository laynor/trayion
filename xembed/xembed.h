/*
 * xembed.h
 * 
 * XEmbed protocol implementation.
 * 
 * Copyright (c) 2008 Paulo Matias
 * Copyright (c) 2004 Matthew Reppert
 * 
 * Use of this code is permitted under the code of the GNU General
 * Public License version 2 (and only version 2).
 */

#ifndef XEMBED_XEMBED_H
#define XEMBED_XEMBED_H


/*
 * This struct holds XEMBED information for a window.
 */
struct xembed_info {
	unsigned long version;
	unsigned long flags;
};


/*
 * The version of the XEMBED protocol that this library supports.
 */
#define XEMBED_VERSION	0


/*
 * Flags for _XEMBED_INFO
 */
#define XEMBED_MAPPED	(1 << 0)
#define XEMBED_INFO_FLAGS_ALL	1


/*
 * XEBMED messages
 */
#define XEMBED_EMBEDDED_NOTIFY		0
#define XEMBED_WINDOW_ACTIVATE		1
#define XEMBED_WINDOW_DEACTIVATE	2
#define XEMBED_REQUEST_FOCUS		3
#define XEMBED_FOCUS_IN			4
#define XEMBED_FOCUS_OUT		5
#define XEMBED_FOCUS_NEXT		6
#define XEMBED_FOCUS_PREV		7
/* 8-9 were used for XEMBED_GRAB_KEY/XEMBED_UNGRAB_KEY */
#define XEMBED_MODALITY_ON		10
#define XEMBED_MODALITY_OFF		11
#define XEMBED_REGISTER_ACCELERATOR	12
#define XEMBED_UNREGISTER_ACCELERATOR	13
#define XEMBED_ACTIVATE_ACCELERATOR	14

/*
 * Details for XEMBED_FOCUS_IN
 */
#define XEMBED_FOCUS_CURRENT		0
#define XEMBED_FOCUS_FIRST		1
#define XEMBED_FOCUS_LAST		2


/*
 * Modifiers field for XEMBED_REGISTER_ACCELERATOR
 */
#define XEMBED_MODIFIER_SHIFT	(1 << 0)
#define XEMBED_MODIFIER_CONTROL	(1 << 1)
#define XEMBED_MODIFIER_ALT	(1 << 2)
#define XEMBED_MODIFIER_SUPER	(1 << 3)
#define XEMBED_MODIFIER_HYPER	(1 << 4)


/*
 * Flags for XEMBED_ACTIVATE_ACCELERATOR
 */
 #define XEMBED_ACCELERATOR_OVERLOADED	(1 << 0)



int xembed_event_is_xembed_event (Display *disp, XEvent *ev);
int xembed_forward_event (Display *disp, Window client, XEvent *ev);
int xembed_send_message (Display *disp, Window to_wind,
			  long message, long d1, long d2, long d3);
int xembed_get_info (Display *disp, Window wind, struct xembed_info *info);
int xembed_unembed_window (Display *disp, Window child);

int xembed_embedded_notify (Display *disp, Window client, Window embedder,
			    long version);

int xembed_window_activate (Display *disp, Window client);
int xembed_window_deactivate (Display *disp, Window client);
int xembed_focus_in (Display *disp, Window client, long focus_type);
int xembed_focus_out (Display *disp, Window client);
int xembed_focus_next (Display *disp, Window client);
int xembed_focus_prev (Display *disp, Window client);

int xembed_register_accelerator (Display *disp, Window client, long accel_id,
				 long keysym, long modifiers);
int xembed_unregister_accelerator (Display *disp, Window client, long id);
int xembed_activate_accelerator (Display *disp, Window client, long id,
				 long flags);

int xembed_modality_on (Display *disp, Window client);
int xembed_modality_off (Display *disp, Window client);


#endif
