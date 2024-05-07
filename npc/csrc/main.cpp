#include <nvboard.h>
#include TOP_NAME_H_FILE    // Defined in npc/Makefile
#include "verilated.h"
#include "verilated_vcd_c.h"

// Configuration of whether use tracing or sequential logic
// #define _DO_TRACE
// #define _SEQUENTIAL_LOGIC

#define MAX_CYCLES 1e2
#ifdef _SEQUENTIAL_LOGIC
const int MAX_SIM_TIME = (MAX_CYCLES) * 2;
#else
const int MAX_SIM_TIME = (MAX_CYCLES);
#endif

static TOP_NAME *top;   // Defined in npc/Makefile
VerilatedContext* contextp;
VerilatedVcdC* tfp;

#ifdef _NVBOARD
void nvboard_bind_all_pins(TOP_NAME *top);
#endif

void status_change() {
    top->eval();
#ifdef _DO_TRACE
    tfp->dump(contextp->time());
#endif
#ifdef _NVBOARD
    nvboard_update();
#endif
    contextp->timeInc(1);
}

#ifdef _SEQUENTIAL_LOGIC
void single_cycle() {

    top->clk = 0; status_change();
    top->clk = 1; status_change();
}

void reset(int n) {
    top->reset = 1;
    while(n-- > 0) single_cycle();
    top->reset = 0;
}
#endif

int main(int argc, char **argv)
{
    contextp = new VerilatedContext;
    contextp->commandArgs(argc, argv);
    top = new TOP_NAME{contextp};
#ifdef _DO_TRACE
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    // tfp->dumpvars(1, "t");  // trace 1 level under "t"
    tfp->open(TRACE_NAME);
#endif

#ifdef _NVBOARD
    nvboard_bind_all_pins(top);
    nvboard_init();
#endif

#ifdef _SEQUENTIAL_LOGIC
    reset(10);
#endif

    while (!contextp->gotFinish() && contextp->time() < MAX_SIM_TIME) {
        top->s = 1;
        top->w = rand() % 2;
        status_change();
    }

#ifdef _DO_TRACE
    tfp->close();
    delete tfp;
#endif
    delete top;
    delete contextp;
#ifdef _NVBOARD
    nvboard_quit();
#endif
    return 0;
}
