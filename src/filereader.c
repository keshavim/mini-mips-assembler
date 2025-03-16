#include "filereader.h"

char **string_split(char *src, char *delim) {
  char **result = malloc(sizeof(char *) * 10);

  char *tok = strtok(src, delim);
  int idx = 0;
  while (tok) {
    *(result + idx++) = tok;
    tok = strtok(NULL, delim);
  }
  *(result + idx++) = NULL;

  return result;
}

void free_string(char **s) { free(s); }

/* read a character form src, convert character to hex HexNumber
 * and add to buf number with bit or. When number is 32 bits, add
 * to file and reset buf.
 *
 * using hex number pramater to return left over bits.
 *
 * */
static void data_to_hex(FILE *dest, const char *src, HexNumber *buf) {
  for (const char *p = src; *p != '\0'; p++) {
    buf->num |= ((unsigned int)*p & 0xFF) << buf->shifts * 8;
    buf->shifts += 1;
    if (buf->shifts == 4) {
      fprintf(dest, "%08lx\n", buf->num);
      buf->num = 0;
      buf->shifts = 0;
    }
  }
  // adding the null
  buf->num |= ('\0' & 0xFF) << buf->shifts * 8;
  buf->shifts += 1;
  if (buf->shifts == 4) {
    fprintf(dest, "%08lx\n", buf->num);
    buf->num = 0;
    buf->shifts = 0;
  }
}

/*  store the data bytes from asm_file in to a text file as
 *  32 bit hex numbers. return labels for the numebers to be
 *  used with generate text file.
 * */
int64_t generate_data_file(FILE *asm_file, Larray *arr) {
  FILE *data_f;
  data_f = fopen("bin/data.txt", "w");
  if (data_f == NULL) {
    printf("Error opening file data\n");
    return 1;
  }

  char line[MAX_LINE_LENGTH];
  HexNumber hex_num = {0};
  size_t offset = 0;
  while (fgets(line, sizeof(line), asm_file)) {
    if (strncmp(line, ".text", 5) == 0) {
      break;
    }

    // seperating the string on the line
    // currently works for just asciiz
    char *data_str = strstr(line, ".asciiz");
    if (data_str == NULL)
      continue;
    data_str = strchr(data_str, '"') + 1;
    char *end = data_str;
    // checking if " is the end of the string or a inner string
    while (*((end = strchr(end, '"')) - 1) == '\\')
      ;
    *end = '\0';

    // geting name of data
    char *label_str = line;
    first_non_space(label_str, line);
    line[strcspn(line, ":")] = '\0';

    // adding to file and label array
    label_add(arr, label_str, offset, DATA_ADDRESS);
    offset += strlen(data_str) + 1;
    data_to_hex(data_f, data_str, &hex_num);
  }
  fclose(data_f);
  return ftell(asm_file);
}

int is_label(char *str) {
  char *s = str;
  while (*s != '\0') {
    if (*s == ' ') {
      *s = '\0';
      return 0;
    } else if (*s == ':') {
      *s = '\0';
      return 1;
    }
    s++;
  }
  return 0;
}

/*
 *
 * */
void generate_labels(FILE *asm_file, int64_t text_start, Larray *arr) {
  char line[MAX_LINE_LENGTH];
  size_t address = 0;
  arr->text_start = arr->current;
  while (fgets(line, sizeof(line), asm_file)) {
    // seperating line to just the instruction
    char *start = line;
    first_non_space(start, line);
    line[strcspn(line, "#\n")] = '\0';
    // blank lines are not added
    if (*start == '\0')
      continue;

    if (is_label(start)) {
      label_add(arr, start, address, TEXT_ADDRESS);
      continue;
    }
    // will make better in the future
    // needs to work with other instrucitons
    if (strcmp(start, "la") == 0) {
      address += 4;
    }
    address += 4;
  }
}

int64_t convert_instruction(char **instrs) {

  int64_t index = INSTRUCTION_GET(instrs[0]);
  if (index == -1) {
    return -1;
  }
  const struct Instruction *instruction = &instruction_list[index];
  // todo: add a psudo case
  if (instruction->type & IT_Register) {
    fflush(stdout);
    return rtype_to_hex(0, instrs[2], instrs[3], instrs[1], 0,
                        instruction->value);
    // sets the strings to the corect pointers for the specific type of
    // immediate
  }
  if (instruction->type & IT_Immideate) {

    size_t op = instruction->value;
    char *rs;
    char *rt;
    char *im;

    if (instruction->type & IT_Branch) {
      rs = instrs[1];
      rt = instrs[2];
      im = instrs[3];

    } else if (instruction->type & IT_Store) {
      rt = instrs[1];

      char **tmp = string_split(instrs[2], "()");
      if (tmp[1] == NULL) {
        rs = tmp[0];
        im = "0";
      } else {
        im = tmp[0];
        rs = tmp[1];
      }
      free(tmp);

    } else if (instruction->type & IT_Load) {
      rs = "$zero";
      rt = instrs[1];
      im = instrs[2];

      // normal immediate
    } else {
      rs = instrs[2];
      rt = instrs[1];
      im = instrs[3];
    }
    return itype_to_hex(op, rs, rt, im);
  }
  if (instruction->type & IT_Jump) {
    return jtype_to_hex(instruction->value, instrs[1]);
  }
  if (instruction->type & IT_Special) {
    return instruction->value;
  }

  return 0;
}

void psudo_to_hex(char **in) {}

static int instruction_to_hex(FILE *dest, char *src, Larray *arr) {

  char line_cpy[strlen(src) + 1];
  strcpy(line_cpy, src);

  char **instrs = string_split(src, DELIM_INSTR);

  const struct Instruction *in = &instruction_list[INSTRUCTION_GET(instrs[0])];

  // will add later
  if (in->type & (IT_Psudo | IT_Multi | IT_Label))
    return 1;
  size_t hex = convert_instruction(instrs);
  fprintf(dest, "%08lx%-4s|%-4s%s\n", hex, "", "", line_cpy);
  return 0;
}
int generate_text_file(FILE *asm_file, int64_t text_start, Larray *arr) {
  size_t t_label_start = arr->current;
  generate_labels(asm_file, text_start, arr);

  FILE *text_f;
  text_f = fopen("bin/text.txt", "w");
  if (text_f == NULL) {
    printf("Error opening file data\n");
    return 1;
  }

  fseek(asm_file, text_start, SEEK_SET);

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), asm_file)) {
    char *p;
    first_non_space(p, line);
    size_t len = strlen(p);

    p[strcspn(p, DELIM_END_LINE)] = '\0';
    // line is empty, a comment, or a label
    if ((*p == '\0') || (label_getaddress(arr, p) != -1)) {
      continue;
    }

    if (instruction_to_hex(text_f, p, arr) != 0) {
      // fprintf(stderr, "Error: %s\n", line);
      // return 1;
    }
  }
  fclose(text_f);

  return 0;
}
