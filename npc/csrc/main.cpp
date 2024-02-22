#include <nvboard.h>
#include "Vtop.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

// TOP_NAME defined in npc/Makefile
static TOP_NAME *top;

const unsigned int MAX_SIM_TIME = 1e6;

void nvboard_bind_all_pins(TOP_NAME *top);

int main(int argc, char **argv)
{
    VerilatedContext* contextp = new VerilatedContext;
    contextp->commandArgs(argc, argv);
    top = new TOP_NAME{contextp};
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    // tfp->dumpvars(1, "t");  // trace 1 level under "t"
    tfp->open("TRACE_NAME");

    nvboard_bind_all_pins(top);
    nvboard_init();

    while (!contextp->gotFinish() && contextp->time() < MAX_SIM_TIME) {
        contextp->timeInc(1);
        int a = rand() & 1;
        int b = rand() & 1;
        top->a = a;
        top->b = b;
        top->eval();
        tfp->dump(contextp->time());
        printf("a = %d, b = %d, f = %d\n", a, b, top->f);
        nvboard_update();
        assert(top->f == (a ^ b));
    }
    tfp->close();
    delete top;
    delete contextp;
    nvboard_quit();
    return 0;
}
