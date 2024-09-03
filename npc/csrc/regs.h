#ifndef __REGS_H__
#define __REGS_H__

#ifndef CONFIG_RV64
typedef uint32_t word_t;
#else
typedef uint64_t word_t;
#endif

void isa_reg_display(TOP_NAME *top);
word_t isa_reg_str2val(TOP_NAME *top, const char *s, bool *success);


#endif