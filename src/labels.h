#ifndef STRARRENA_H
#define STRARRENA_H

#include "common.h"

#define NAME_SIZE 100
typedef struct Label {
  size_t address;
  char name[NAME_SIZE];
} Label;

#define MAX_LABELS 1000 // doing it by stack for now
typedef struct Label_Array {
  size_t size;
  size_t current;
  size_t text_start;
  Label labels[MAX_LABELS];
} Larray;

void label_add(Larray *arr, char *name, size_t offset, size_t type);
size_t label_getaddress(Larray *arr, char *name);

#endif
