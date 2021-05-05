#ifndef _UTILS_H_
#define _UTILS_H_

#include <stddef.h>

typedef int (*callback_t)(void *, void *);


typedef struct path_store {
  struct path_store *next;
  char data[1];
} path_store_t;

path_store_t *path_store_init(char const *str, size_t len);
void path_store_free(path_store_t *list);
void path_store_add(path_store_t **where, char const *str, size_t len);
void path_store_remove(path_store_t **where);


#endif
