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
}

void iringbuf_put(IRingBuf *iringbuf, Decode *s) {
  iringbuf->decode[iringbuf->now_at++] = *s;
  iringbuf->now_at %= IRINGBUF_SIZE;
}

void iringbuf_print(IRingBuf *iringbuf) {
  printf("Recently execued %d instructions:\n", IRINGBUF_SIZE);
  int start = iringbuf->now_at;
  for (int i = start; i != start; i = (i + 1) % IRINGBUF_SIZE) {
    print_decode(&iringbuf->decode[i]);
  }
}

#endif