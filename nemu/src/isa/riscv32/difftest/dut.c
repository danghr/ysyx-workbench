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

#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

// CPU state for NEMU
extern CPU_state cpu;

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  // ref_r: State copied from Spike
  // pc: PC of the current instruction

  for (int i = 0; i < MUXDEF(CONFIG_RV32E, 16, 32); i++) {
    if (ref_r->gpr[i] != cpu.gpr[i]) {
      Log("Different value in register %s at pc = " FMT_WORD ", reference = " FMT_WORD ", NEMU = " FMT_WORD,
          reg_name(i), pc, ref_r->gpr[i], cpu.gpr[i]);
      return false;
    }
  }

  return true;
}

void isa_difftest_attach() {
}
