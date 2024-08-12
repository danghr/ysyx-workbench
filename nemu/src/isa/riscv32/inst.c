/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

// Instructions with complex design are implemented by functions.
static void branch_exec(Decode *s, bool cond, word_t imm);
static void jal_exec(Decode *s, int rd, word_t imm);
static void jalr_exec(Decode *s, int rd, word_t src1, word_t imm);

enum {
  TYPE_R, TYPE_I, TYPE_U, TYPE_S, TYPE_B, TYPE_J,
  TYPE_N, // none
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
#define immB() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 12 | \
                            BITS(i, 7, 7) << 11 | \
                            BITS(i, 30, 25) << 5 | \
                            BITS(i, 11, 8) << 1); } while(0)
#define immJ() do { *imm = (SEXT(BITS(i, 31, 31), 1) << 20 | \
                            BITS(i, 19, 12) << 12 | \
                            BITS(i, 20, 20) << 11 | \
                            BITS(i, 30, 21) << 1); } while(0)

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  /* Select `rd`, `rs1` and `rs2` from fixed location*/
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  /* Put the value of `rd`, `src1`, `src2` and `imm` in corresponding variables 
     according to the type of the instruction */
  switch (type) {
    case TYPE_R: src1R(); src2R();         break;
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_B: src1R(); src2R(); immB(); break;
    case TYPE_J:                   immJ(); break;
  }
}

static int decode_exec(Decode *s) {
  int rd = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);

  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);
#define MULT_DOUBLE_LENGTH_SIGNED MUXDEF(CONFIG_ISA64, __int128_t, __int64_t)
#define MULT_DOUBLE_LENGTH_UNSIGNED MUXDEF(CONFIG_ISA64, __uint128_t, __uint64_t)
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd) = (word_t)((MULT_DOUBLE_LENGTH_SIGNED)src1 * (MULT_DOUBLE_LENGTH_SIGNED)src2));
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(rd) = (sword_t)src1 / (sword_t)src2); // signed
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(rd) = (src1 < src2) ? 1 : 0); // unsigned
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);
  /* Note that shift operations only shift "by the shift amount held in the lower 5 bits of register rs2".
     Refer to "2.4.2. Integer Register-Register Operations". */
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = src1 << BITS(src2, 4, 0));

  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd) = (src1 < imm) ? 1 : 0);  // unsigned
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);

  /* Note that in a shift-by-a-constant instructions, only imm[4:0] is used as the shift amount, and
     inst[30] is used to distinguish between SRLI and SRAI.
     Refer to "2.4.1 Integer Register-Immediate Instructions" in RISC-V manual. */
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = (sword_t)src1 >> BITS(imm, 4, 0));  // arithmetic

  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = SEXT(Mr(src1 + imm, 4), 32));
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));

  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));

  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, branch_exec(s, (src1 == src2), imm));
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, branch_exec(s, (src1 != src2), imm));
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, branch_exec(s, ((sword_t)src1 < (sword_t)src2), imm));  // signed

  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, jal_exec(s, rd, imm));
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, jalr_exec(s, rd, src1, imm));

  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  // `snpc` now equals to `pc + 4` as `inst_fetch` increments the first variable by 4
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}

void branch_exec(Decode *s, bool cond, word_t imm) {
  if (cond)
    s->dnpc = s->pc + imm;
}

void jal_exec(Decode *s, int rd, word_t imm) {
  s->dnpc = s->pc + imm;
  // Use s->snpc to reflect the original design goal in ISA document
  // "JAL stores the address of the instruction following the jump ('pc'+4) into register rd."
  R(rd) = s->snpc;
  // Note that "R(0) = 0" ensures the correctness of register $zero
  // so we do not need to handle the case seperately
}

void jalr_exec(Decode *s, int rd, word_t src1, word_t imm) {
  // According to the document, "setting the least-significant bit of the result to zero"
  s->dnpc = (vaddr_t)(src1 + imm) & (~(vaddr_t)1);
  // Put this line after the above line to ensure the correctness when rd == rs1
  R(rd) = s->snpc;
}
