#include "Vtop.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

const unsigned int MAX_SIM_TIME = 100;
unsigned int sim_time = 0;

int main(int argc, char** argv) {
    Verilated::mkdir("logs");
    VerilatedContext* contextp = new VerilatedContext;
    contextp->commandArgs(argc, argv);
    Vtop* top = new Vtop{contextp};
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    // tfp->dumpvars(1, "t");  // trace 1 level under "t"
    tfp->open("obj_dir/t_trace_top/simx.vcd");
    while (!contextp->gotFinish() && contextp->time() < MAX_SIM_TIME) {
        contextp->timeInc(1);
        int a = rand() & 1;
        int b = rand() & 1;
        top->a = a;
        top->b = b;
        top->eval();
        tfp->dump(contextp->time());
        printf("a = %d, b = %d, f = %d\n", a, b, top->f);
        assert(top->f == (a ^ b));
        sim_time++;
    }
    tfp->close();
    delete top;
    delete contextp;
    return 0;
}
