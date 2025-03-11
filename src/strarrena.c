#include "strarrena.h"

void strarrena_init(Arrena *src) {
  src->size = DEFAULT_DATA_SIZE;
  src->current_size = 0;
  src->data = malloc(DEFAULT_DATA_SIZE);
}
char *strarrena_add(Arrena *src, char *addend) {
  size_t len = strlen(addend);
  if (src->size <= src->current_size + len) {
    src->size *= 2;
    src->data = realloc(src->data, src->size);
  }

  char *current_p = &src->data[src->current_size];
  strcpy(current_p, addend);
  src->current_size += len + 1;
  return current_p;
}
void arrena_delete_data(Arrena *src) { free(src->data); }
