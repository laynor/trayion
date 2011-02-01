#include <stdlib.h>
#include <string.h>
#include "hidden_list.h"
#include "string_list.h"
#include "ui.h"


struct list_head hidden_list;
int show_hidden = 0;

void load_hidden_list(char *fname)
{
	load_string_list(fname, &hidden_list);
}

void print_hidden_list()
{
	print_string_list(&hidden_list);
}

int is_hidden(Window window)
{
	XClassHint *class_hint;
	struct list_head *n;
	struct string_item *item;
	TRACE((stderr, "ENTERING: is_hidden\n"));
	class_hint = XAllocClassHint();
	if(!XGetClassHint(main_disp, window, class_hint)){
		XFree(class_hint);
		return 0;
	}
	list_for_each(n, &hidden_list) {
		item = list_entry(n, struct string_item, string_list);
		if(!strcmp(item->info, class_hint->res_class)){
			XFree(class_hint);
			return 1;
			break;
		}
	}
	XFree(class_hint);
	TRACE((stderr, "LEAVING: is_hidden\n"));
	return 0;
}
