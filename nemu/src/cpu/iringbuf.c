#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/iringbuf.h>

#ifdef CONFIG_ITRACE

/**
 * We utilize `Decode->logbuf` to print the instruction information
 * However, we choose to store the whole `Decode` struct in the ring buffer
 * to get ready for further extension in the future.
 */

static void print_decode(Decode *s) {
  printf("%s\n", s->logbuf);
}

void iringbuf_init(IRingBuf *iringbuf) {
  iringbuf->now_at = 0;
  iringbuf->one_loop = false;
}

void iringbuf_put(IRingBuf *iringbuf, Decode *s) {
  iringbuf->decode[iringbuf->now_at++] = *s;
  if (iringbuf->now_at == IRINGBUF_SIZE) {
    iringbuf->one_loop = true;
  }
  iringbuf->now_at %= IRINGBUF_SIZE;
}

void iringbuf_print(IRingBuf *iringbuf) {
  printf("Recently execued %d instructions:\n", iringbuf->one_loop ? IRINGBUF_SIZE : iringbuf->now_at);
  int start = iringbuf->now_at;
  if (iringbuf->one_loop) {
    for (int i = start; i != start; i = (i + 1) % IRINGBUF_SIZE) {
      print_decode(&iringbuf->decode[i]);
    }
  } else {
    for (int i = 0; i < start; i++) {
      print_decode(&iringbuf->decode[i]);
    }
  }
  
}

#endif