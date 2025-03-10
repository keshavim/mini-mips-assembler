#include "mips_converter.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const struct Instruction instruction_list[] = {
    instruction_init(m_add, TYPE_R),
    instruction_init(m_addiu, TYPE_I),
    instruction_init(m_and, TYPE_R),
    instruction_init(m_andi, TYPE_I),
    instruction_init(m_beq, TYPE_IB),
    instruction_init(m_bne, TYPE_IB),
    instruction_init(m_j, TYPE_J),
    instruction_init(m_lui, TYPE_IL),
    instruction_init(m_lw, TYPE_IS),
    instruction_init(m_or, TYPE_R),
    instruction_init(m_ori, TYPE_I),
    instruction_init(m_slt, TYPE_R),
    instruction_init(m_sub, TYPE_R),
    instruction_init(m_sw, TYPE_IS),
    instruction_init(m_syscall, TYPE_SPECIAL),
    instruction_init(m_blt, TYPE_PSUDO),
    instruction_init(m_la, TYPE_PSUDO),
    instruction_init(m_li, TYPE_PSUDO),
    instruction_init(m_move, TYPE_PSUDO)};

const char *mips_registers[] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0",   "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0",   "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8",   "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

// returns index or -1 if not found
int64_t array_search(const void *key, const void *src, size_t src_len,
                     size_t elem_size, compare_func cmp) {
  const char *base = src;

  for (size_t i = 0; i < src_len; i++) {
    if (cmp(key, base + (i * elem_size)) == 0) {
      return i;
    }
  }
  return -1;
}

int32_t instruction_cmp(const void *a, const void *b) {
  const char *A = (const char *)a;
  const char *B = ((const struct Instruction *)b)->name + 2;
  return strcmp(A, B);
}
int32_t register_cmp(const void *a, const void *b) {
  const char *A = (const char *)a;
  const char *B = *(const char **)b;
  return strcmp(A, B);
}

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

size_t parse_num(const char *str) {
  size_t result = 0;
  char *endptr;
  if (strncmp(str, "0x", 2) == 0 || strncmp(str, "0X", 2) == 0) {
    result = strtoull(str + 2, &endptr, 16);
  } else {
    result = strtoull(str, &endptr, 10);
  }
  if (*endptr != '\0' || (result == 0 && errno == EINVAL)) {
    fprintf(stderr, "Invalid input string\n");
    exit(1);
  }
  return result;
}
int64_t convert_instruction(char **instrs) {
  int64_t index = INSTRUCTION_GET(instrs[0]);
  if (index == -1) {
    return -1;
  }
  const struct Instruction *instruction = &instruction_list[index];
  // todo: add a psudo case
  switch (instruction->type) {
  case TYPE_R:

    // opcode | rs | rt| rd | shamt | funct
    return (0 << 26) | (REGISTER_GET(instrs[2]) << 21) |
           (REGISTER_GET(instrs[3]) << 16) | (REGISTER_GET(instrs[1]) << 11) |
           (0 << 6) | (instruction->value);
  case TYPE_I:
    // opcode | rs | rt| immediate
    return type_i_shift(instruction->value, instrs[2], instrs[1], instrs[3]);
  case TYPE_IB:
    //(opcode) | (rs) | (rt) | immediate
    return type_i_shift(instruction->value, instrs[1], instrs[2], instrs[3]);
  case TYPE_IS: {
    // opcode | base | rt | offset
    char **tmp = string_split(instrs[2], "()");
    char *base;
    char *offset;
    if (tmp[1] == NULL) {
      base = tmp[0];
      offset = "0";
    } else {
      base = tmp[1];
      offset = tmp[0];
    }
    size_t res = type_i_shift(instruction->value, base, instrs[1], offset);
    free(tmp);
    return res;
  }
  case TYPE_IL:
    // opcode | 0 | rt | immediate
    return type_i_shift(instruction->value, 0, instrs[1], instrs[2]);
  case TYPE_J:
    // opcode | instruction_index
    return (instruction->value << 26) | parse_hex(instrs[1]);
  case TYPE_SPECIAL:
    // opcode
    return instruction->value;
  case TYPE_PSUDO:
    return convert_psudo_instruction(instrs, instruction);
  default:
    return -1;
  }
}

int64_t convert_psudo_instruction(char **instrs,
                                  const struct Instruction *inst) {
  switch (inst->value) {
  case m_li: {
    return type_i_shift(m_addiu, "$zero", instrs[1], instrs[2]);
  }
  case m_la:
    return 0;
  case m_blt:
    return 0;
  case m_move:
    return 0;
  default:
    return -1;
  }
}
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
