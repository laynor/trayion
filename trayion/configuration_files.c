#include "configuration_files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "systray.h"
#include "string_list.h"
#include "sorted_classes.h"
#include "hidden_list.h"

void ensure_cfg_exists_or_create(char *path) {
	FILE* fp;
	char *hrpath = home_relative_path(path);
	fp = fopen(hrpath, "a+");
	
	if(!fp){
		perror(NULL);
		exit(1);
	}
	free(hrpath);
	fclose(fp);
}
void ensure_default_cfg()
{
	struct stat s;
	int res;
	char *path;
	int mode = F_OK|R_OK;
	path = home_relative_path(".trayion");
	if(access(path, mode)){
		if (errno == ENOENT)
			mkdir(path, 0755);
		else{
			fprintf(stderr, "cannot access ~/.trayion: ");
			perror(NULL);
			exit(1);
		}
	}
	free(path);
	ensure_cfg_exists_or_create(SORTING_CONFIG_FILE);
	ensure_cfg_exists_or_create(HIDING_CONFIG_FILE);
}

void reload_config_files()
{
	static int initialized=0;
	ensure_default_cfg();
	load_sorting_config();
	load_hiding_config();
	if (initialized){
		recalc_window_ranks();
		/* sort_systray_list(); */
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
	char *path = home_relative_path(SORTING_CONFIG_FILE);
	load_sorted_classes_list(path);
	free(path);
}
void load_hiding_config()
{
	char *path = home_relative_path(HIDING_CONFIG_FILE);
	load_hidden_list(path);
	free(path);
}
