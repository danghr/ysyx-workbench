#include <nvboard.h>
#include <Vtop.h>

// TOP_NAME in this file is defined by macro TOPNAME in npc/Makefile

static TOP_NAME dut;                        

void nvboard_bind_all_pins(TOP_NAME* top);

int main() {
  nvboard_bind_all_pins(&dut);
  nvboard_init();

  while(true) {
    int a = rand() & 1;
    int b = rand() & 1;
    dut.a = a;
    dut.b = b;
    dut.eval();
    nvboard_update();
  }
  nvboard_quit();
}
