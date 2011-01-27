#ifndef SORTED_CLASSES_H
#define SORTED_CLASSES_H

#include "list.h"
struct sorted_classes_item {
   char *class;
   struct list_head sorted_classes_list;
};

extern struct list_head sorted_classes_list;

int window_rank(Window window);
void print_sorted_classes_list();
void load_sorted_classes_list(char *fname);
#endif
