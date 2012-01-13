#ifndef _CONFIGURATION_FILES_H_
#define _CONFIGURATION_FILES_H_

#define SORTING_CONFIG_FILE ".trayion/sorted-iconlist.conf"
#define HIDING_CONFIG_FILE ".trayion/hidden-iconlist.conf"
#define DUMP_FILE "/tmp/sorted-iconlist.conf"

void ensure_default_cfg();
char* home_relative_path(char *rpath);
void load_sorting_config();
void load_hiding_config();
void reload_config_files();


#endif /* _CONFIGURATION_FILES_H_ */
