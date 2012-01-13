#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "trace.h"
#include "ui.h"
#include "string_list.h"
#include "sorted_classes.h"



int unknown_class_count = 0;
int no_class_count = 0;
struct list_head sorted_classes_list;


void load_sorted_classes_list(char *fname)
{
	load_string_list(fname, &sorted_classes_list);
}

void print_sorted_classes_list()
{
	print_string_list(&sorted_classes_list);
}


int window_rank(Window window)
{
	XClassHint *class_hint;
	struct list_head *n;
	struct string_item *item;
	TRACE((stderr, "ENTERING: window_rank\n"));
	class_hint = XAllocClassHint();
	// No window class
	if(!XGetClassHint(main_disp, window, class_hint)){
		XFree(class_hint);
		no_class_count++;
		return INT_MAX - no_class_count;
	}
	int i = 0;
	list_for_each(n, &sorted_classes_list) {
		item = list_entry(n, struct string_item, string_list);
		if(!strcmp(item->info, class_hint->res_class)){
			XFree(class_hint);
			return TRAYION_UNHIDDEN_BASE_RANK + i;
		}
		i++;
	}
	XFree(class_hint);
	TRACE((stderr, "LEAVING: window_rank\n"));
	unknown_class_count++;
	return TRAYION_UNKNOWN_BASE_RANK - unknown_class_count;
}

int pushed_left_rank(int rank)
{
	return TRAYION_LEFT_HIDDEN_BASE_RANK + rank - TRAYION_UNHIDDEN_BASE_RANK;
}
