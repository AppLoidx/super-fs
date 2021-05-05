#include "../inc/xfs/utils.h"
#include <stdlib.h>
#include <string.h>

path_store_t *path_store_init(char const *str, size_t len) {
  path_store_t *result = malloc(sizeof(path_store_t *) + len + 1);
  memcpy(result->data, str, len);
  result->data[len] = 0;
  return result;
}

void path_store_free(path_store_t *store) {
  if (store) {
    path_store_t *next = store->next;
    free(store);
    path_store_free(next);
  }
}

void path_store_add(path_store_t **where, char const *str, size_t len) {
  path_store_t *new_ = path_store_init(str, len);
  new_->next = *where;
  *where = new_;
}

void path_store_remove(path_store_t **where) {
  path_store_t *next = (*where)->next;
  (*where)->next = NULL;
  path_store_free(*where);
  *where = next;
}
