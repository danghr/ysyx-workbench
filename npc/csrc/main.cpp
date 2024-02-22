#include <nvboard.h>
#include TOP_NAME_H_FILE    // Defined in npc/Makefile
#include "verilated.h"
#include "verilated_vcd_c.h"

#define MAX_CYCLES 1e5
#define MAX_SIM_TIME (MAX_CYCLES * 2)

static TOP_NAME *top;   // Defined in npc/Makefile
VerilatedContext* contextp;
VerilatedVcdC* tfp;


void nvboard_bind_all_pins(TOP_NAME *top);

void status_change() {
    top->eval();
    tfp->dump(contextp->time());
    nvboard_update();
    contextp->timeInc(1);
}

void single_cycle() {
    top->clk = 0; status_change();
    top->clk = 1; status_change();
}

void reset(int n) {
    top->rst = 1;
    while(n-- > 0) single_cycle();
    top->rst = 0;
}

int main(int argc, char **argv)
{
    contextp = new VerilatedContext;
    contextp->commandArgs(argc, argv);
    top = new TOP_NAME{contextp};
    Verilated::traceEverOn(true);
    tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    // tfp->dumpvars(1, "t");  // trace 1 level under "t"
    tfp->open(TRACE_NAME);

    nvboard_bind_all_pins(top);
    nvboard_init();

    while (!contextp->gotFinish() && contextp->time() < MAX_SIM_TIME) {
        single_cycle();
    }
    tfp->close();
    delete top;
    delete contextp;
    delete tfp;
    nvboard_quit();
    return 0;
}
