#include "mips_converter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define DELIM " ,#\n\\"
#define GREEN "\033[0;32m"
#define RED "\033[31m"
#define RESET "\033[0m"
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
      char *p = strstr(line, ".asciiz");
      if (!p)
        continue;

      p = strchr(p, '"') + 1;
      char *end = p; // last char of string
      while (*((end = strchr(end, '"')) - 1) == '\\')
        ;
      *end = '\0';

      data_to_hex(data_f, p, &hex_num);
    } else {
      fprintf(text_f, "%s\n", line);
    }
  }

  fclose(asm_f);
  fclose(data_f);
  fclose(text_f);
  return 0;
}
