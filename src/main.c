#include "labels.h"
#include "mips_converter.h"

#define MAX_LINE_LENGTH 1000
#define DELIM " ,#\n\\"
#define GREEN "\033[0;32m"
#define RED "\033[31m"
#define RESET "\033[0m"

int test() {

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
      label_array.byte_offset = 0;
      continue;
    }

    if (is_data) {
      // seperating the string on the line
      // currently works for just asciiz
      char *data_str = strstr(line, ".asciiz");
      if (!data_str)
        continue;
      data_str = strchr(data_str, '"') + 1;
      char *end = data_str; // last char of string
      while (*((end = strchr(end, '"')) - 1) == '\\')
        ;
      *end = '\0';
      char *label_str = line;
      first_non_space(label_str, line);
      line[strcspn(line, ":")] = '\0';
      datalabel_add(label_str, strlen(data_str) + 1, DATA_START);
      data_to_hex(data_f, data_str, &hex_num);

      // will need to add more 0's after to fillup the other data addresses
    } else {
      // seperating line to just the instruction
      char *start = line;
      first_non_space(start, line);
      line[strcspn(line, "#")] = '\0';
      if (*start == '\0')
        continue;

      char line_cpy[strlen(start) + 1];
      strcpy(line_cpy, start);
      char **split_line = string_split(line_cpy, DELIM);

      int64_t index = INSTRUCTION_GET(split_line[0]);
      if (index == -1) {

        start[strcspn(start, ":")] = '\0';
        textlabel_add(start, TEXT_START);
        fprintf(text_f, "LABEL  |  %s\n", start);
        continue;
      }

      size_t itype = (instruction_list[index].type);
      if (itype == TYPE_J || itype == TYPE_IB){
        label_array.byte_offset += 4;
        fprintf(text_f, "%s\n", start);
      }
      else if (itype == TYPE_PSUDO) {

        size_t num[3] = {0};
        convert_psudo_instruction(split_line, num);
        for (int i = 0; num[i] != 0; i++) {
          fprintf(text_f, "%08lx   |   %s\n", *num, start);
          label_array.byte_offset += 4;
        }
      } else {
        int64_t num = convert_instruction(split_line);
        fprintf(text_f, "%08lx   |   %s\n", num, start);
        label_array.byte_offset += 4;
      }
    }
  }

  fclose(asm_f);
  fclose(data_f);
  fclose(text_f);
  return 0;
}

Larray label_array = {0};

int main(int argc, char *argv[]) {
  test();
  for (int i = 0; i < label_array.current; i++) {
    printf("%s  %08lx\n", label_array.labels[i].name,
           label_array.labels[i].address);
  }
}
