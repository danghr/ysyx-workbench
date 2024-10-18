#ifndef __CPU__IRINGBUF_H__
#define __CPU__IRINGBUF_H__

#include <common.h>
#include <cpu/decode.h>

#ifdef CONFIG_ITRACE

#define IRINGBUF_SIZE 32
typedef struct
{
  int now_at;
  bool one_loop;
  Decode decode[IRINGBUF_SIZE];
} IRingBuf;

void iringbuf_init(IRingBuf *iringbuf);
void iringbuf_put(IRingBuf *iringbuf, Decode *s);
void iringbuf_print(IRingBuf *iringbuf);

#endif

#endif
