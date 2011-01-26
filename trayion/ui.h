/*
 * ui.h
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

#ifndef WMSYSTRAY_UI_H
#define WMSYSTRAY_UI_H


/*extern int wmaker;*/
extern int iconsize;

int init_ui(char *app_name, int argc, char **argv);
void cleanup_ui();
void wmsystray_handle_signal (int signum);
void wmsystray_event_loop();
void wmsystray_resize(int width, int height);

extern char *display_string;
/*extern char *geometry_string;*/
extern Display *main_disp;
extern Window main_wind, icon_wind, sel_wind, draw_wind;
/*extern char * wmsystray_xpm[];*/

#endif
