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
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

extern CPU_state cpu;

void isa_reg_display() {
  for (int i = 0;
       i < MUXDEF(CONFIG_RVE, 16, 32) /* See src/riscv32/include/isa_def.h */;
       i++
  ) {
    /* 64-bit registers for RV64. See include/common.h and RISC-V ISA I Sec. 4.1 */
#ifdef CONFIG_RV64  
    printf("%-4s  0x%016x  %lld\n", regs[i], cpu.gpr[i], cpu.gpr[i]);
#else
    printf("%-4s  0x%08x  %d\n", regs[i], cpu.gpr[i], cpu.gpr[i]);
#endif
  }

#ifdef CONFIG_RV64
  printf("pc    0x%016x\n", cpu.pc);
#else
  printf("pc    0x%08x\n", cpu.pc);
#endif
}

word_t isa_reg_str2val(const char *s, bool *success) {
  // We can accept both `$name` and `name` as register names.
  // We can accept both `$ra` and `$x1` as register names.
  // Also, we can accept `$x0`, `$0` and `$zero` for the zero register.
  char *reg_name = (char *)s;
  if (s[0] == '$')
    reg_name++;
  if (strcmp(reg_name, "pc") == 0) {
    *success = true;
    return cpu.pc;
  }
  if (reg_name[0] == 'x') {
    // Directly extract number of the register
    char **endptr = NULL;
    long long idx = strtol(reg_name + 1, endptr, 10);
    if (!(*reg_name != '\0' && **endptr == '\0')) {
      *success = false;
      printf("Invalid register name: %s. Register names starting with 'x' can only be a number.\n", s);
      return 0;
    }
    if (idx < 0 || idx >= MUXDEF(CONFIG_RVE, 16, 32)) {
      *success = false;
      printf("Invalid register name: %s. Register index out of range.\n", s);
      return 0;
    }
    *success = true;
    return cpu.gpr[idx];
  }
  if (strcmp(reg_name, "zero") == 0 || strcmp(reg_name, "0") == 0) {
    *success = true;
    return 0;
  }
  for (int i = 0; i < MUXDEF(CONFIG_RVE, 16, 32); i++) {
    if (strcmp(reg_name, regs[i]) == 0) {
      *success = true;
      return cpu.gpr[i];
    }
  }
  *success = false;
  printf("Invalid register name: %s.\n", s);
  return 0;
}
