#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "ui.h"
#include "sorted_classes.h"


struct list_head sorted_classes_list;
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}
void load_sorted_classes_list(char *fname)
{
	char s[256];
	FILE *fptr;
	struct sorted_classes_item *item;

	INIT_LIST_HEAD(&sorted_classes_list);

	if ((fptr = fopen(fname, "r"))) {
		while (fgets(s, 256*sizeof(char), fptr)){
			item = malloc(sizeof(struct sorted_classes_item));
			item->class = malloc((1+strlen(s))*sizeof(char));
			strcpy(item->class, s);
			item->class = trimwhitespace(item->class);
			list_add_tail(&item->sorted_classes_list, &sorted_classes_list);
		}
	}
}

void print_sorted_classes_list()
{
	struct sorted_classes_item *item;
	struct list_head *n;
	list_for_each (n, &sorted_classes_list) {
		item = list_entry(n, struct sorted_classes_item, sorted_classes_list);
		printf("%s\n", item->class);
	}
}

int window_rank(Window window)
{
	XClassHint class_hint;
	struct list_head *n;
	struct sorted_classes_item *item;
	XGetClassHint(main_disp, window, &class_hint);
	int i = 0;
	list_for_each(n, &sorted_classes_list) {
		item = list_entry(n, struct sorted_classes_item, sorted_classes_list);
		if(!strcmp(item->class, class_hint.res_name)){
			return i;
			break;
		}
		i++;
	}
	return INT_MAX;
}
