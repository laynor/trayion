#ifndef SORTED_CLASSES_H
#define SORTED_CLASSES_H

#include <X11/Xlib.h>
#include "list.h"
#include <limits.h>

#define TRAYION_LEFT_HIDDEN_BASE_RANK 0
#define TRAYION_UNKNOWN_BASE_RANK 20000
#define TRAYION_UNHIDDEN_BASE_RANK 10000


extern struct list_head sorted_classes_list;

int window_rank(Window window);
void print_sorted_classes_list();
void load_sorted_classes_list(char *fname);
int pushed_left_rank(int rank);
#endif
