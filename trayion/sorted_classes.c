#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "trace.h"
#include "ui.h"
#include "string_list.h"
#include "sorted_classes.h"


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
	if(!XGetClassHint(main_disp, window, class_hint)){
		XFree(class_hint);
		return INT_MAX;
	}
	int i = 0;
	list_for_each(n, &sorted_classes_list) {
		item = list_entry(n, struct string_item, string_list);
		if(!strcmp(item->info, class_hint->res_class)){
			XFree(class_hint);
			return i;
			break;
		}
		i++;
	}
	XFree(class_hint);
	TRACE((stderr, "LEAVING: window_rank\n"));
	return INT_MAX;
}
