#include <memory/init_image.h>
#include <memory/paddr.h>
#include "Vysyx_24070014_top_module.h"
#include "Vysyx_24070014_top_module__Syms.h"
#include "verilated_vcd_c.h"
#include "verilated_dpi.h"

// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
// static const uint32_t img [] = {
//   0x00000297,  // auipc t0,0
//   0x00028823,  // sb  zero,16(t0)
//   0x0102c503,  // lbu a0,16(t0)
//   0x00100073,  // ebreak (used as nemu_trap)
//   0xdeadbeef,  // some data
// };
static const uint32_t img [] = {
    0x00100093, // addi x1, x0, 1
    0x00108113, // addi x2, x1, 1
    0x00a10a13, // addi x20, x2, 10
    0x00108093, // addi x1, x1, 1
    0x00108093, // addi x1, x1, 1
    0x00100073, // ebreak
};

void init_default_image() {
  /* Load built-in image. */
  memcpy(guest_to_host(RESET_VECTOR), img, sizeof(img));
}