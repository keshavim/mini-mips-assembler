#ifndef MIPS_CONVERTER_H
#define MIPS_CONVERTER_H

#include "common.h"
#include "labels.h"
#define m_add 0x20
#define m_addiu 0x09
#define m_and 0x24
#define m_andi 0x0c
#define m_beq 0x04
#define m_bne 0x05
#define m_j 0x02
#define m_lui 0x0f
#define m_lw 0x23
#define m_or 0x25
#define m_ori 0x0d
#define m_slt 0x2a
#define m_sub 0x22
#define m_sw 0x2b
#define m_syscall 0x0c

// psudo
#define m_blt -1
#define m_la -2
#define m_li -3
#define m_move -4

#define instruction_init(name, type) {(name), (type), (#name)}
#define INSTRUCTION_SIZE 19 // will be bigger with more added
#define REGISTER_SIZE 32

#define IT_Register (1 << 0)
#define IT_Immideate (1 << 1)
#define IT_Jump (1 << 2)
#define IT_Branch (1 << 3)
#define IT_Load (1 << 4)
#define IT_Store (1 << 5)
#define IT_Special (1 << 6)
#define IT_Psudo (1 << 7)
#define IT_Label (1 << 8)
#define IT_Multi (1 << 9)

// this makes a global array
extern const struct Instruction {
  size_t value;
  size_t type;
  const char *name;
} instruction_list[INSTRUCTION_SIZE];

extern const char *mips_registers[REGISTER_SIZE];

typedef int (*compare_func)(const void *, const void *);

// helper funciton macros
//  instruction and register getters
#define INSTRUCTION_GET(key)                                                   \
  array_search(key, instruction_list, INSTRUCTION_SIZE,                        \
               sizeof(instruction_list[0]), instruction_cmp)
#define REGISTER_GET(key)                                                      \
  array_search(key, mips_registers, REGISTER_SIZE, sizeof(char *), register_cmp)

// string
#define first_non_space(p, str)                                                \
  for ((p) = (str); (p) != NULL && *(p) != '\0' && isspace(*(p)); (p)++)

#define parseNum(s, e) (strtoull((s), (e), 0) & 0xFFFF)

// simple macros for geting the type of the hex number of instruction
#define rtype_to_hex(op, rs, rt, rd, shamt, val)                               \
  ((op) << 26) | (REGISTER_GET(rs) << 21) | (REGISTER_GET(rt) << 16) |         \
      (REGISTER_GET(rd) << 11) | ((shamt) << 6) | (val)

#define itype_to_hex(op, rs, rt, im)                                           \
  (((op) << 26) | (REGISTER_GET(rs) << 21) | (REGISTER_GET(rt) << 16) |        \
   parseNum((im), NULL))

#define jtype_to_hex(op, im) ((op) << 26 | parseNum((im), NULL))
int64_t array_search(const void *key, const void *src, size_t src_len,
                     size_t elem_size, compare_func cmp);

int32_t instruction_cmp(const void *a, const void *b);
int32_t register_cmp(const void *a, const void *b);

#endif
