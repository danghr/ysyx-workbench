#ifndef __REGS_H__
#define __REGS_H__

#include "main.h"


void isa_reg_display(TOP_NAME *top);
word_t isa_reg_str2val(TOP_NAME *top, const char *s, bool *success);

#endif