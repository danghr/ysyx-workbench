#include <nvboard.h>
#include TOP_NAME_H_FILE // Defined in npc/Makefile
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "regs.h"

const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void isa_reg_display(TOP_NAME *top)
{
  uint32_t *gpr = top->top_signal_regfile;
  uint32_t pc = top->TOP_PC;

  for (int i = 0; i < 32; i++)
  {
    /* 64-bit registers for RV64. */
#ifdef CONFIG_RV64
    printf("%-4s  0x%016x  %lld\n", regs[i], gpr[i], gpr[i]);
#else
    printf("%-4s  0x%08x  %d\n", regs[i], gpr[i], gpr[i]);
#endif
  }

#ifdef CONFIG_RV64
  printf("pc    0x%016x\n", pc);
#else
  printf("pc    0x%08x\n", pc);
#endif
}

word_t isa_reg_str2val(TOP_NAME *top, const char *s, bool *success)
{
  uint32_t *gpr = top->top_signal_regfile;
  uint32_t pc = top->TOP_PC;

  // We can accept both `$name` and `name` as register names.
  // We can accept both `$ra` and `$x1` as register names.
  // Also, we can accept `$x0`, `$0` and `$zero` for the zero register.
  char *reg_name = (char *)s;
  if (s[0] == '$')
    reg_name++;
  if (strcmp(reg_name, "pc") == 0)
  {
    *success = true;
    return pc;
  }
  if (reg_name[0] == 'x')
  {
    // Directly extract number of the register
    char *endptr;
    long long idx = strtol(reg_name + 1, &endptr, 10);
    if (!(*reg_name != '\0' && *endptr == '\0'))
    {
      *success = false;
      printf("Invalid register name: %s. Register names starting with 'x' can only be a number.\n", s);
      return 0;
    }
    if (idx < 0 || idx >= 32)
    {
      *success = false;
      printf("Invalid register name: %s. Register index out of range.\n", s);
      return 0;
    }
    *success = true;
    return gpr[idx];
  }
  if (strcmp(reg_name, "zero") == 0 || strcmp(reg_name, "0") == 0)
  {
    *success = true;
    return 0;
  }
  for (int i = 0; i < 32; i++)
  {
    if (strcmp(reg_name, regs[i]) == 0)
    {
      *success = true;
      return gpr[i];
    }
  }
  *success = false;
  printf("Invalid register name: %s.\n", s);
  return 0;
}
