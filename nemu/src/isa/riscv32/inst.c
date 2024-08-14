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

/**
 * Simulate branch execution and update the dynamic next PC.
 * 
 * @param s The current Decode structure. If branch is taken, the function will modify `s->dnpc` so that the
 *          branch target is to be executed.
 * @param cond The condition of the branch. If true, the branch is taken.
 * @param imm The immediate value of the branch instruction. Address of branch target will be `s->pc + imm`.
 */
static void branch_exec(Decode *s, bool cond, word_t imm);

/**
 * Simulate jump and link (`jal`) operation. Write the return address to the destination register R(rd).
 * 
 * @param s The current Decode structure. The function will modify `s->dnpc` so that the next instruction to be
 *          executed is the address to be jumped.
 * @param rd The destination register index to write the return address, which is the address of the next static
 *           instruction (i.e., `pc + 4`).
 * @param imm The immediate value of the jump instruction. The address of the jump target will be `s->pc + imm`.
 */
static void jal_exec(Decode *s, int rd, word_t imm);

/**
 * Simulate jump and link register (`jalr`) operation. Write the return address to the destination register R(rd).
 * 
 * @param s The current Decode structure. The function will modify `s->dnpc` so that the next instruction to be
 *         executed is the address to be jumped.
 * @param rd The destination register index to write the return address, which is the address of the next static
 *         instruction (i.e., `pc + 4`).
 * @param src1 The base address to jump from.
 * @param imm The immediate value of the jump instruction. The address of the jump target will be `src1 + imm`.
 */
static void jalr_exec(Decode *s, int rd, word_t src1, word_t imm);

/**
 * Simulate division operation and write the result to the destination register R(rd).
 *
 * @param rd The destination register index.
 * @param src1 The dividend.
 * @param src2 The divisor.
 * @param is_signed Treat the operands as signed. Otherwise, treat them as unsigned.
 * @param out_remainder Write the remainder to the destination register. Otherwise, write the quotient.
 */
static void div_exec(int rd, word_t src1, word_t src2, bool is_signed, bool out_remainder);


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

  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd) = src1 + src2);
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd) = src1 - src2);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd) = src1 | src2);
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd) = src1 & src2);
  /* Note that shift operations only shift "by the shift amount held in the lower 5 bits of register rs2".
     Refer to "2.4.2. Integer Register-Register Operations". */
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd) = src1 << BITS(src2, 4, 0));
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd) = src1 >> BITS(src2, 4, 0));          // logical
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd) = (sword_t)src1 >> BITS(src2, 4, 0)); // arithmetic
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, R(rd) = ((sword_t)src1 < (sword_t)src2) ? 1 : 0); // signed
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, R(rd) = (src1 < src2) ? 1 : 0);

  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);
  // TODO: ori
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << BITS(imm, 4, 0));
  /* Note that in a shift-by-a-constant instructions, only imm[4:0] is used as the shift amount, and
     inst[30] is used to distinguish between SRLI and SRAI.
     Refer to "2.4.1 Integer Register-Immediate Instructions" in RISC-V manual. */
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(rd) = (word_t)src1 >> BITS(imm, 4, 0));   // logical
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = (sword_t)src1 >> BITS(imm, 4, 0));  // arithmetic
  // TODO: slti   // signed
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd) = (src1 < imm) ? 1 : 0);

  /* Note that the memory-reading operation of load instructions are still executed even if `rd=0`.
     Refer to "2.6. Load and Store Instructions", which states that "Loads with a destination of x0 must still raise any exceptions
     and cause any other side effects even though the load value is discarded." */
  // TODO: lb   // signed
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(rd) = SEXT(Mr(src1 + imm, 2), 16)); // signed
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = SEXT(Mr(src1 + imm, 4), 32)); // signed (ready for RV64)
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(rd) = Mr(src1 + imm, 2));

  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2));
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));

  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, branch_exec(s, (src1 == src2), imm));
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, branch_exec(s, (src1 != src2), imm));
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, branch_exec(s, ((sword_t)src1 < (sword_t)src2), imm));  // signed
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, branch_exec(s, ((sword_t)src1 >= (sword_t)src2), imm)); // signed
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, branch_exec(s, (src1 < src2), imm));
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, branch_exec(s, (src1 >= src2), imm));

  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, jal_exec(s, rd, imm));
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, jalr_exec(s, rd, src1, imm));

  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm); // Set lower 12 bits to 0

  // TODO: ecall
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0

  // M extension for integer multiplication and division
  typedef MUXDEF(CONFIG_ISA64, __int128_t, int64_t) double_sword_t;
  typedef MUXDEF(CONFIG_ISA64, __uint128_t, uint64_t) double_word_t __attribute__((unused));
  /* Result of multiplication of 2's complement numbers is the same as treating the number as unsigned ones. 
     Safely let the result overflows to get the lower 32 bits. */
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd) = (word_t)(src1 * src2));
  /* For singed operands in `mulh` and `mulhsu`, we need to convert the number first into `sword_t` then `double_sword_t`.
     This is to ensure that the sign extension is correct when extending the number into higher bits, as directly converting `word_t`
     into `double_word_t` treats the original number as unsigned numbers so that no sign extension will be done. */
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, 
    R(rd) = (word_t)(((double_sword_t)(sword_t)src1 * (double_sword_t)(sword_t)src2) >> MUXDEF(CONFIG_ISA64, 64, 32))); // signed * signed
  // TODO: mulu     // unsigned * unsigned
  // TODO: mulhsu   // signed * unsigned
  /* Result of division of 2's complement numbers is NOT the same as treating the number as unsigned ones. */
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, div_exec(rd, src1, src2, true, false));   // signed
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, div_exec(rd, src1, src2, false, false));
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, div_exec(rd, src1, src2, true, true));    // signed
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, div_exec(rd, src1, src2, false, true));

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


/**
 * Check if the instruction is aligned to 4 bytes.
 * 
 * @param pc The program counter of the instruction.
 * @return True if the instruction is aligned to 4 bytes, false otherwise.
 */
void check_inst_alignment(vaddr_t pc) {
  if (unlikely((pc & 0x3) != 0)) {
    printf("Unaligned instruction address: " FMT_WORD "\n", pc);
    // Raise an exception if the target address is not aligned to 4 bytes
    // TODO: Raise an exception
    assert(0);
  }
}

void branch_exec(Decode *s, bool cond, word_t imm) {
  if (cond) {
    vaddr_t target = s->pc + imm;
    check_inst_alignment(target);
    s->dnpc = target;
  }
}

void jal_exec(Decode *s, int rd, word_t imm) {
  vaddr_t target = s->pc + imm;
  check_inst_alignment(target);
  s->dnpc = target;
  // Use s->snpc to reflect the original design goal in ISA document
  // "JAL stores the address of the instruction following the jump ('pc'+4) into register rd."
  R(rd) = s->snpc;
  // Note that "R(0) = 0" ensures the correctness of register $zero
  // so we do not need to handle the case seperately
}

void jalr_exec(Decode *s, int rd, word_t src1, word_t imm) {
  // According to the document, "setting the least-significant bit of the result to zero"
  vaddr_t target = (vaddr_t)(src1 + imm) & (~(vaddr_t)1);
  check_inst_alignment(target);
  s->dnpc = target;
  // Put this line after the above line to ensure the correctness when rd == rs1
  R(rd) = s->snpc;
}

void div_exec(int rd, word_t src1, word_t src2, bool is_signed, bool out_remainder) {
  word_t quotient;
  word_t remainder;
  
  // Handle special cases
  // Refer to "13.2. Division Operations"
  if (unlikely(src2 == 0)) {
    // Division by zero
    quotient = (word_t)(-1);  // All bits set to 1
    remainder = src1;
  } else if (unlikely(is_signed && 
             (src1 == (word_t)(MUXDEF(CONFIG_ISA64, 0x8000000000000000, 0x80000000)) &&
              src2 == (word_t)-1))) {
    // Overflow
    quotient = src1;
    remainder = 0;
  } else {
    quotient = is_signed ? ((sword_t)src1 / (sword_t)src2) : (src1 / src2);
    remainder = is_signed ? ((sword_t)src1 % (sword_t)src2) : (src1 % src2);
  }
  
  if (out_remainder) R(rd) = remainder;
  else R(rd) = quotient;
}
