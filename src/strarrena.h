#ifndef STRARRENA_H
#define STRARRENA_H

#include "common.h"

#define DATA_START 0x10010000
#define TEXT_START 0x00400000

#define DEFAULT_DATA_SIZE 100
typedef struct Arrena {
  size_t size;
  size_t current_size;
  char *data;
} Arrena;

#define NAME_SIZE 100
typedef struct Label {
  char *ptr;
  char name[NAME_SIZE];
} Label;

void strarrena_init(Arrena *src);
char *strarrena_add(Arrena *src, char *addend);
void arrena_delete_data(Arrena *src);

// todo label functions

#endif
