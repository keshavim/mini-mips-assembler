#include "filereader.h"

// will need to add more 0's after to fillup the other data addresses
/*} else {*/
/*  // seperating line to just the instruction*/
/*  char *start = line;*/
/*  first_non_space(start, line);*/
/*  line[strcspn(line, "#")] = '\0';*/
/*  if (*start == '\0')*/
/*    continue;*/
/**/
/*  char line_cpy[strlen(start) + 1];*/
/*  strcpy(line_cpy, start);*/
/*  char **split_line = string_split(line_cpy, DELIM);*/
/**/
/*  int64_t index = INSTRUCTION_GET(split_line[0]);*/
/*  if (index == -1) {*/
/**/
/*    start[strcspn(start, ":")] = '\0';*/
/*    textlabel_add(start, TEXT_START);*/
/*    fprintf(text_f, "LABEL  |  %s\n", start);*/
/*    continue;*/
/*  }*/
/**/
/*  size_t itype = (instruction_list[index].type);*/
/*  if (itype == TYPE_J || itype == TYPE_IB) {*/
/*    label_array.byte_offset += 4;*/
/*    fprintf(text_f, "%s\n", start);*/
/*  } else if (itype == TYPE_PSUDO) {*/
/**/
/*    size_t num[3] = {0};*/
/*    convert_psudo_instruction(split_line, num);*/
/*    for (int i = 0; num[i] != 0; i++) {*/
/*      fprintf(text_f, "%08lx   |   %s\n", *num, start);*/
/*      label_array.byte_offset += 4;*/
/*    }*/
/*  } else {*/
/*    int64_t num = convert_instruction(split_line);*/
/*    fprintf(text_f, "%08lx   |   %s\n", num, start);*/
/*    label_array.byte_offset += 4;*/
/*  }*/

int test_instructions() {

  FILE *asm_f;
  char line[MAX_LINE_LENGTH];

  asm_f = fopen("tests.txt", "r");
  if (asm_f == NULL) {
    printf("Error opening file asm\n");
    return 1;
  }

  while (fgets(line, sizeof(line), asm_f)) {
    line[strcspn(line, "\n")] = '\0';

    char *end = NULL;
    size_t expected = strtoll(line, &end, 16);
    line[strcspn(line, DELIM_END_LINE)] = '\0';
    char **sline = string_split(end, DELIM_INSTR);

    size_t result = convert_instruction(sline);
    if (expected != result) {
      printf("%s   %08lx    %08lx\n", sline[0], expected, result);
      return 1;
    }
    fflush(stdout);
    free_string(sline);
  }

  fclose(asm_f);
  printf("complete");
  return 0;
}

int test_asm_file() {

  Larray label_array;

  FILE *asm_f;
  char line[MAX_LINE_LENGTH];

  asm_f = fopen("EvenOrOdd.asm", "r");
  if (asm_f == NULL) {
    printf("Error opening file asm\n");
    return 1;
  }

  int64_t p = generate_data_file(asm_f, &label_array);
  generate_text_file(asm_f, p, &label_array);
  for (int i = 0; i < label_array.current; i++) {
    printf("%s  %08lx\n", label_array.labels[i].name,
           label_array.labels[i].address);
  }
  fclose(asm_f);
  return 0;
}

int main(int argc, char *argv[]) {
  test_asm_file();

  return 0;
}
