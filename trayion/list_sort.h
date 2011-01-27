#ifndef LIST_SORT_H
#define LIST_SORT_H

#include "list.h"

void list_sort(struct list_head *head, int (*cmp)(struct list_head *a, struct list_head *b));

#endif
