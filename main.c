#include "mips_converter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define DELIM " ,#\n\\"
#define GREEN "\033[0;32m"
#define RED "\033[31m"
#define RESET "\033[0m"

#define first_non_space(p, str)                                                \
  for ((p) = (str); (p) != NULL && *(p) != '\0' && isspace(*(p)); (p)++)

int main(int argc, char *argv[]) {

  FILE *asm_f;
  char line[MAX_LINE_LENGTH];

  asm_f = fopen("EvenOrOdd.asm", "r");
  if (asm_f == NULL) {
    printf("Error opening file asm\n");
    return 1;
  }

  FILE *data_f;
  data_f = fopen("bin/data.txt", "w");
  if (data_f == NULL) {
    printf("Error opening file data\n");
    return 1;
  }
  FILE *text_f;
  text_f = fopen("bin/text.txt", "w");
  if (text_f == NULL) {
    printf("Error opening file text\n");
    return 1;
  }

  int is_data = 1;
  HexNumber hex_num = {0};
  while (fgets(line, sizeof(line), asm_f)) {
    if (strncmp(line, ".text", 5) == 0) {
      is_data = 0;
      continue;
    }

    if (is_data) {
      // seperating the string on the line
      // currently works for just asciiz
      char *start = strstr(line, ".asciiz");
      if (!start)
        continue;
      start = strchr(start, '"') + 1;
      char *end = start; // last char of string
      while (*((end = strchr(end, '"')) - 1) == '\\')
        ;
      *end = '\0';

      data_to_hex(data_f, start, &hex_num);
      // will need to add more 0's after to fillup the other data addresses
    } else {
      // seperating line to just the instruction
      char *start = line;
      first_non_space(start, line);
      line[strcspn(line, "#")] = '\0';
      if (*start != '\0')
        fprintf(text_f, "%s", start);
    }
  }

  fclose(asm_f);
  fclose(data_f);
  fclose(text_f);
  return 0;
}
