#include "filereader.h"
#include "common.h"
#include "instructions.h"

// string
static inline char *first_non_space(const char *str) {
  if (str == NULL) {
    return NULL; // Ha:warn(""); null input safely
  }
  while (*str != '\0' && isspace((unsigned char)*str)) {
    str++;
  }
  return (char *)str;
}
char **string_split(const char *src, char *delim) {
  char *cpy = malloc(strlen(src) + 1);
  strcpy(cpy, src);
  char **result = malloc(sizeof(char *) * 10);

  char *tok = strtok(cpy, delim);
  int idx = 0;
  while (tok) {
    *(result + idx++) = tok;
    tok = strtok(NULL, delim);
  }
  *(result + idx++) = NULL;

  return result;
}

void free_string(char **s) {
  for (int i = 0; s[i] != NULL; i++) {
    free(s[i]);
  }
  free(s);
}

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
int64_t generate_data_file(const char *dest, FILE *asm_file, Larray *arr) {
  FILE *data_f;
  data_f = fopen(dest, "w");
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
    char *label_str = first_non_space(line);
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
    char *start = first_non_space(line);
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
    address += 4;
  }
}

int64_t convert_instruction(char **instrs, Larray *labels) {

  int64_t index = INSTRUCTION_GET(instrs[0]);
  if (index == -1) {
    return -1;
  }
  const struct Instruction *instruction = &instruction_list[index];
  // todo: add a psudo case

  if (instruction->type & IT_Register) {
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
    size_t l = label_getaddress(labels, instrs[1]);
    size_t address = (l != -1) ? l : parseNum(instrs[1], NULL);
    return jtype_to_hex(instruction->value, address);
  }
  if (instruction->type & IT_Special) {
    return instruction->value;
  }

  return 0;
}

#define HEX_SIZE 8
// returns the hex and new instruction for a psudo instruction
char *convert_psudo(char **instrs, Larray *labels) {

  const struct Instruction *in = &instruction_list[INSTRUCTION_GET(instrs[0])];
  if (in->type & (IT_Multi | IT_Label))
    return NULL;
  int len = 0;
  char *return_string = NULL;
  if (in->value == m_li) {
    // make new word
    char new_instrs_string[MAX_LINE_LENGTH];
    snprintf(new_instrs_string, sizeof(new_instrs_string),
             "addiu %s, $zero, 0x%08llx", instrs[1], parseNum(instrs[2], NULL));

    /// split to parts
    int size = HEX_SIZE + sizeof(new_instrs_string) + 3;
    return_string = malloc(size);
    char **new_instrs = string_split(new_instrs_string, DELIM_INSTR);

    // get hex and return string of hex and new instruction
    size_t hex = convert_instruction(new_instrs, labels);
    free(new_instrs);
    snprintf(return_string, size, "%08lx %s", hex, new_instrs_string);

  } else if (in->value == m_move) {

    // make new word
    char new_instrs_string[MAX_LINE_LENGTH];
    snprintf(new_instrs_string, sizeof(new_instrs_string), "addu %s, $zero, %s",
             instrs[1], instrs[2]);
    /// split to parts
    int size = HEX_SIZE + sizeof(new_instrs_string) + 3;
    return_string = malloc(size);
    char **new_instrs = string_split(new_instrs_string, DELIM_INSTR);

    // get hex and return string of hex and new instruction
    size_t hex = convert_instruction(new_instrs, labels);
    free(new_instrs);
    snprintf(return_string, size, "%08lx %s", hex, new_instrs_string);
  }
  return return_string;
}

static int instruction_to_hex(FILE *dest, char *src, Larray *arr) {

  char **instrs = string_split(src, DELIM_INSTR);

  const struct Instruction *in = &instruction_list[INSTRUCTION_GET(instrs[0])];

  // will add later
  if (in->type & IT_Psudo) {
    char *res = convert_psudo(instrs, arr);
    free(instrs);
    fprintf(dest, "%s%-2s|%-2s%s\n", res, "", "", src);
    free(res);
    return (0);
  }

  size_t hex = convert_instruction(instrs, arr);
  free(instrs);
  fprintf(dest, "%08lx%-2s|%-2s%s\n", hex, "", "", src);
  return 0;
}
int generate_text_file(const char *dest, FILE *asm_file, int64_t text_start,
                       Larray *arr) {
  size_t t_label_start = arr->current;
  generate_labels(asm_file, text_start, arr);

  FILE *text_f;
  text_f = fopen(dest, "w");
  if (text_f == NULL) {
    printf("Error opening file data\n");
    return 1;
  }

  fseek(asm_file, text_start, SEEK_SET);

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), asm_file)) {
    char *p = first_non_space(line);
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
