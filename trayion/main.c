/*
 * main.c
 * 
 * General "top-level" driver code; get wmsystray running and processing
 * events.
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
#include <signal.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "ui.h"
#include "systray.h"
#include "version.h"
#include "configuration_files.h"


void usage();
void parse_cmd_line (int argc, char **argv);

static int error_handler(Display *d, XErrorEvent *e) {
	return 0;
}

void save_pid_file()
{
	FILE *fptr;

	fptr = fopen(home_relative_path(".trayion/trayion.pid"), "w");
	fprintf(fptr, "%d", getpid());
	fclose(fptr);
}

int main(int argc, char **argv) {
	struct sigaction act, oldact;

	reload_config_files();

	XSetErrorHandler(error_handler);

	parse_cmd_line(argc, argv);
	act.sa_handler = &wmsystray_handle_signal;
	act.sa_flags = 0;
	sigaction (SIGTERM, &act, &oldact);
	sigaction (SIGINT, &act, &oldact);
	sigaction (SIGUSR1, &act, &oldact);
	sigaction (SIGUSR2, &act, &oldact);
	sigaction (SIGALRM, &act, &oldact);

	if (init_ui ("wmsystray", argc, argv) != 0) {
		printf ("Could not connect to X server!\n");
		return 0;
	}

	if (init_systray()) {
		fprintf (stderr, "Could not get systray!\n");
		cleanup_ui();
		return 0;
	}

	save_pid_file();
	wmsystray_event_loop();

	cleanup_systray();
	cleanup_ui();
	return 0;
}



/*
 * usage
 *
 * Print command usage information.
 */
void usage() {
	printf ("%s\n", PACKAGE_STRING);
	printf ("Usage: %s [OPTION] ... \n\n", PACKAGE_TARNAME);
	printf ("\t-display <display>\t\t\tX display to connect to\n");
	printf ("\t-iconsize <pixels>\t\t\ticon size in pixels\n");
	printf ("\t-hidden-items-left\t\t\tdisplay hidden icons on the left side of the tray (default)\n");
	printf ("\t-keep-hidden-items-position\t\t\tkeep the hidden icons position as specified by the sorting config file\n");
	printf ("\n\nReport bugs to: %s\n", PACKAGE_BUGREPORT);
}



/*
 * parse_cmd_line
 *
 * Parse command line arguments.
 */
void parse_cmd_line (int argc, char **argv) {
	int i;

	display_string = getenv("DISPLAY");

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-iconsize") == 0) {
			iconsize = atoi(argv[i + 1]);
			if(!iconsize)
				iconsize = 14;
			i++;
		} else if (strcmp(argv[i], "-display") == 0) {
			display_string = argv[i + 1];
			i++;
		} else if (strcmp(argv[i], "-h") == 0 ||
			   strcmp(argv[i], "--help") == 0)
		{
			usage();
			exit(0);
		} else if (strcmp(argv[i], "-hidden-items-left")) {
			place_hidden_items_on_the_left = 1;
		} else if (strcmp(argv[i], "-keep-hidden-items-position")) {
			place_hidden_items_on_the_left = 0;
		}
#if 0
		else if (strcmp(argv[i], "--nowm") == 0) {
			wmaker = 0;
		}
#endif
	}
}
