#ifndef SORTED_CLASSES_H
#define SORTED_CLASSES_H

#include "list.h"

extern struct list_head sorted_classes_list;

int window_rank(Window window);
void print_sorted_classes_list();
void load_sorted_classes_list(char *fname);
#endif
