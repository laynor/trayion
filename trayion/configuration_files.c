#include "configuration_files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "systray.h"
#include "string_list.h"
#include "sorted_classes.h"
#include "hidden_list.h"

void reload_config_files()
{
	static int initialized=0;
	load_sorting_config();
	load_hiding_config();
	if (initialized){
		recalc_window_ranks();
		sort_systray_list();
	}
	initialized = 1;
}
char* home_relative_path(char *rpath)
{
	char *homedir;
	char *apath;
	homedir = getenv("HOME");
	apath = malloc((strlen(rpath) + strlen(homedir) + 2) * sizeof(char));
	apath[0]='\0';
	strcpy(apath, homedir);
	strcat(apath, "/");
	strcat(apath, rpath);
	return apath;
}

void load_sorting_config()
{
	char *path = home_relative_path(".trayion/trayion-sorted-iconlist.txt");
	load_sorted_classes_list(path);
	free(path);
}
void load_hiding_config()
{
	char *path = home_relative_path(".trayion/trayion-hidden-iconlist.txt");
	load_hidden_list(path);
	free(path);
}
