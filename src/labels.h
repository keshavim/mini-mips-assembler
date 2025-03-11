#ifndef STRARRENA_H
#define STRARRENA_H

#include "common.h"

#define DATA_START 0x10010000
#define TEXT_START 0x00400000

#define NAME_SIZE 100
typedef struct Label {
  size_t address;
  char name[NAME_SIZE];
} Label;

typedef struct Label_Array {
  size_t size;
  size_t current;
  size_t byte_offset;
  Label labels[NAME_SIZE];
} Larray;

void label_add(Larray *arr, char *name, size_t data_size, size_t type);
size_t label_getaddress(Larray *arr, char *name);

#endif
