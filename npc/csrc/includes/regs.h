#ifndef __REGS_H__
#define __REGS_H__

#include "common.h"
#include "Vysyx_24070014_top_module___024root.h"

#define TOP_PC rootp->ysyx_24070014_top_module__DOT__pc

void isa_reg_display(TOP_NAME *top);
word_t isa_reg_str2val(TOP_NAME *top, const char *s, bool *success);

#endif