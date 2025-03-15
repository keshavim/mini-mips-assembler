#include "filereader.h"
#include "labels.h"

/* read a character form src, convert character to hex HexNumber
 * and add to buf number with bit or. When number is 32 bits, add
 * to file and reset buf.
 *
 * using hex number pramater to return left over bits.
 *
 * */
void data_to_hex(FILE *dest, const char *src, HexNumber *buf) {
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
