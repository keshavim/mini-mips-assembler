#include "filereader.h"

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

    size_t result = convert_instruction(sline, NULL);
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

  asm_f = fopen("tests/EvenOrOdd.asm", "r");
  if (asm_f == NULL) {
    printf("Error opening file asm\n");
    return 1;
  }

  int64_t p = generate_data_file("tests/eoo_data.txt", asm_f, &label_array);
  generate_text_file("tests/eoo_text.txt", asm_f, p, &label_array);
  fclose(asm_f);
  return 0;
}

int main(int argc, char *argv[]) {
  test_asm_file();
  /*char move[] = "move $t4, $v0";*/
  /*char **split = string_split(move, DELIM_INSTR);*/
  /*char *s = convert_psudo(split, NULL);*/
  /*if (s != NULL)*/
  /*  printf("%s\n", s);*/
  /*free(s);*/
  return 0;
}
