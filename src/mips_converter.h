#ifndef MIPS_CONVERTER_H
#define MIPS_CONVERTER_H

#include "common.h"
#include "strarrena.h"
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

enum InstructionType {
  TYPE_R,
  TYPE_I,
  TYPE_IB,
  TYPE_IL,
  TYPE_IS,
  TYPE_J,
  TYPE_PSUDO,
  TYPE_SPECIAL
};

// this makes a global array
extern const struct Instruction {
  size_t value;
  enum InstructionType type;
  const char *name;
} instruction_list[INSTRUCTION_SIZE];

extern const char *mips_registers[REGISTER_SIZE];

typedef struct HexNumber {
  size_t num;
  size_t shifts;
} HexNumber;

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

#define parse_hex(n) (parse_num((n)) & 0xFFFF)

#define type_i_shift(op, rs, rt, im)                                           \
  (((op) << 26) | (REGISTER_GET(rs) << 21) | (REGISTER_GET(rt) << 16) |        \
   parse_hex(im))

// functions definitions
int64_t array_search(const void *key, const void *src, size_t src_len,
                     size_t elem_size, compare_func cmp);

int32_t instruction_cmp(const void *a, const void *b);
int32_t register_cmp(const void *a, const void *b);

// array of pointers must be freed
char **string_split(char *src, char *delim);
void free_string(char **s);

size_t parse_num(const char *str);

int64_t convert_instruction(char **instrs);

int64_t convert_psudo_instruction(char **instrs,
                                  const struct Instruction *inst);

void data_to_hex(FILE *dest, const char *src, HexNumber *buf);

#endif
