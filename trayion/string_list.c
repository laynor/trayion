#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "string_list.h"

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
void load_string_list(char *fname, struct list_head *string_list)
{
	char s[256];
	FILE *fptr;
	struct string_item *item;

	INIT_LIST_HEAD(string_list);

	if ((fptr = fopen(fname, "r"))) {
		while (fgets(s, 256*sizeof(char), fptr)){
			if (s[0] == '#') continue;
			item = malloc(sizeof(struct string_item));
			item->info = malloc((1+strlen(s))*sizeof(char));
			strcpy(item->info, s);
			item->info = trimwhitespace(item->info);
			list_add_tail(&item->string_list, string_list);
		}
	}
}

void print_string_list(struct list_head *string_list)
{
	struct string_item *item;
	struct list_head *n;
	list_for_each (n, string_list) {
		item = list_entry(n, struct string_item, string_list);
		printf("%s\n", item->info);
	}
}
