#include <nvboard.h>
#include "Vtop.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#define TRACE_NAME "./build/t_trace_top/top.vcd"
// TOP_NAME in this file is defined by macro TOPNAME in npc/Makefile
static TOP_NAME *dut;

const unsigned int MAX_SIM_TIME = 100;

void nvboard_bind_all_pins(TOP_NAME *top);

int main(int argc, char **argv)
{
    VerilatedContext *contextp = new VerilatedContext;
    contextp->commandArgs(argc, argv);
    dut = new TOP_NAME{contextp};
    Verilated::traceEverOn(true);
    VerilatedVcdC *tfp = new VerilatedVcdC;

    nvboard_bind_all_pins(dut);
    nvboard_init();

    dut->trace(tfp, 99);
    tfp->open(TRACE_NAME);

    while (!contextp->gotFinish() && contextp->time() < MAX_SIM_TIME)
    {
        contextp->timeInc(1);
        int a = rand() & 1;
        int b = rand() & 1;
        dut->a = a;
        dut->b = b;
        dut->eval();
        tfp->dump(contextp->time());
        printf("a = %d, b = %d, f = %d\n", dut->a, dut->b, dut->f);
        nvboard_update();
        assert(dut->f == (dut->a ^ dut->b));
    }
    tfp->close();
    nvboard_quit();
    delete contextp;
    delete tfp;
    delete dut;
    return 0;
}
