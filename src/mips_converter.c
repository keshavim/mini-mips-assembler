#include "mips_converter.h"
#include <stdio.h>

#define IT_Register (1 << 0)
#define IT_Immideate (1 << 1)
#define IT_Jump (1 << 2)
#define IT_Branch (1 << 3)
#define IT_Load (1 << 4)
#define IT_Store (1 << 5)
#define IT_Special (1 << 6)
const struct Instruction instruction_list[] = {
    instruction_init(m_add, IT_Register),
    instruction_init(m_addiu, IT_Immideate),
    instruction_init(m_and, IT_Register),
    instruction_init(m_andi, IT_Immideate),
    instruction_init(m_beq, IT_Immideate | IT_Branch),
    instruction_init(m_bne, IT_Immideate | IT_Branch),
    instruction_init(m_j, IT_Jump),
    instruction_init(m_lui, IT_Immideate | IT_Load),
    instruction_init(m_lw, IT_Immideate | IT_Store),
    instruction_init(m_or, IT_Register),
    instruction_init(m_ori, IT_Immideate),
    instruction_init(m_slt, IT_Register),
    instruction_init(m_sub, IT_Register),
    instruction_init(m_sw, IT_Immideate | IT_Store),
    instruction_init(m_syscall, IT_Special),
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

// simple macros for geting the type of the hex number of instruction
#define rtype_to_hex(op, rs, rt, rd, shamt, val)                               \
  ((op) << 26) | (REGISTER_GET(rs) << 21) | (REGISTER_GET(rt) << 16) |         \
      (REGISTER_GET(rd) << 11) | ((shamt) << 6) | (val)

#define itype_to_hex(op, rs, rt, im)                                           \
  (((op) << 26) | (REGISTER_GET(rs) << 21) | (REGISTER_GET(rt) << 16) |        \
   parseNum((im), NULL))

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
  } else if (instruction->type & IT_Immideate) {

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

  } else if (instruction->type & IT_Jump) {
    return (instruction->value << 26) | parseNum(instrs[1], NULL);

  } else if (instruction->type & IT_Special) {
    return instruction->value;
  }

  return 0;
}

// will change completly
void convert_psudo_instruction(char **instrs, size_t *result) {
  int64_t index = INSTRUCTION_GET(instrs[0]);
  if (index == -1) {
    return;
  }
  const struct Instruction *inst = &instruction_list[index];
  switch (inst->value) {
  case m_li: {
    result[0] = itype_to_hex(m_addiu, "$zero", instrs[1], instrs[2]);
    result[1] = 0;
    break;
  }
  case m_la:
    result[0] = 1;
    result[1] = 1;
    result[2] = 0;
    return;
  case m_blt:
    return;
  case m_move:
    return;
  default:
    return;
  }
}
