#include "instructions.h"

const struct Instruction instruction_list[] = {
    instruction_init(m_add, IT_Register),
    instruction_init(m_addu, IT_Register),
    instruction_init(m_addiu, IT_Immideate),
    instruction_init(m_and, IT_Register),
    instruction_init(m_andi, IT_Immideate),
    instruction_init(m_beq, IT_Immideate | IT_Branch | IT_Label),
    instruction_init(m_bne, IT_Immideate | IT_Branch | IT_Label),
    instruction_init(m_j, IT_Jump | IT_Label),
    instruction_init(m_lui, IT_Immideate | IT_Load),
    instruction_init(m_lw, IT_Immideate | IT_Store),
    instruction_init(m_or, IT_Register),
    instruction_init(m_ori, IT_Immideate),
    instruction_init(m_slt, IT_Register),
    instruction_init(m_sub, IT_Register),
    instruction_init(m_sw, IT_Immideate | IT_Store),
    instruction_init(m_syscall, IT_Special),
    instruction_init(m_blt, IT_Psudo | IT_Label | IT_Multi),
    instruction_init(m_la, IT_Psudo | IT_Label | IT_Multi),
    instruction_init(m_li, IT_Psudo),
    instruction_init(m_move, IT_Psudo)};

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
