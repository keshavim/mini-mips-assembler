#include "labels.h"

void label_add(Larray *arr, char *name, size_t offset, size_t type) {
  Label *ptr = &arr->labels[arr->current];
  ptr->address = type + offset;
  strcpy(ptr->name, name);
  arr->current++;
}
size_t label_getaddress(Larray *arr, char *label) {
  for (size_t i = 0; i < arr->current; i++) {
    if (strcmp(arr->labels[i].name, label) == 0) {
      return arr->labels[i].address;
    }
  }
  return -1;
}
