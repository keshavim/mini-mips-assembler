#include "labels.h"

void label_add(Larray *arr, char *name, size_t data_size, size_t type) {
  Label *ptr = &arr->labels[arr->current];
  ptr->address = type + arr->byte_offset;
  strcpy(ptr->name, name);
  if (type == DATA_START)
    arr->byte_offset += data_size;
  arr->current++;
}
size_t label_getaddress(Larray *arr, char *label) {
  for (size_t i = 0; i < arr->current; i++) {
    if (strcmp(arr->labels[i].name, label) == 0) {
      return arr->labels[i].address;
    }
  }
  return 0;
}
