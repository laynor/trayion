#ifndef _HIDDEN_LIST_H_
#define _HIDDEN_LIST_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

extern int show_hidden;

void load_hidden_list();
int is_hidden(Window w);

extern struct list_head hidden_list;

#endif /* _HIDDEN_LIST_H_ */
