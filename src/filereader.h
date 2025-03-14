#ifndef FILE_READER_H
#define FILE_READER_H

#include "common.h"
#include "labels.h"
#include "mips_converter.h"

// used to stor a hexnumber and what point in the shift level it has
typedef struct HexNumber {
  size_t num;
  size_t shifts;
} HexNumber;

void data_to_hex(FILE *dest, const char *src, HexNumber *buf);
int64_t generate_data_file(FILE *asm_file, Larray *arr);
void generate_labels(FILE *asm_file, int64_t text_start, Larray *arr);
#endif
