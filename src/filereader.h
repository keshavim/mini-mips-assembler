#ifndef FILE_READER_H
#define FILE_READER_H

#include "common.h"
#include "instructions.h"

#define DELIM_END_LINE ":#\n"
#define DELIM_INSTR " ,"
#define GREEN "\033[0;32m"
#define RED "\033[31m"
#define RESET "\033[0m"
// used to stor a hexnumber and what point in the shift level it has
typedef struct HexNumber {
  size_t num;
  size_t shifts;
} HexNumber;

int64_t generate_data_file(FILE *asm_file, Larray *arr);
void generate_labels(FILE *asm_file, int64_t text_start, Larray *arr);
int generate_text_file(FILE *asm_file, int64_t text_start, Larray *arr);

int64_t convert_instruction(char **instrs);
// array of pointers must be freed
char **string_split(char *src, char *delim);
void free_string(char **s);
#endif
