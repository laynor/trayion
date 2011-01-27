#ifndef _STRING_LIST_H_
#define _STRING_LIST_H_

#include "list.h"

struct string_item {
   char *info;
   struct list_head string_list;
};

char *trimwhitespace(char *str);
void load_string_list(char *fname, struct list_head *string_list);
void print_string_list(struct list_head *string_list);

#endif /* _STRING_LIST_H_ */


