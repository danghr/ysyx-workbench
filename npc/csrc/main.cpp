#include <nvboard.h>
#include TOP_NAME_H_FILE    // Defined in npc/Makefile
#include "verilated.h"
#include "verilated_vcd_c.h"

// #define _DO_TRACE
// #define _SEQUENTIAL_LOGIC
#define _NVBOARD

// Configuration of whether use tracing or sequential logic
// #define _DO_TRACE
// #define _SEQUENTIAL_LOGIC

#define MAX_CYCLES 1000000
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
    top->clk = 1; status_change();
    top->clk = 0; status_change();
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
    reset(1);
#endif

    // =============================
    // === Begin simulation body ===
    // =============================

    srand(time(NULL));

    // Values to be tested
    const int VALUES_SIZE = 9;
    int values[VALUES_SIZE] = {7, 6, 2, 1, 0, -1, -2, -7, -8};

    // Test add
    top->sel=0;
    for (int i = 0; i < VALUES_SIZE; i++) { 
        top->a = values[i];
        for (int j = 0; j < VALUES_SIZE; j++) {
            top->b = values[j];
            status_change();
            assert (top->y == values[i] + values[j]);
            assert (top->zero == (top->y == 0));
            printf("a = %d, b = %d, a + b = %d\n", values[i], values[j], top->y);
        }
    }

    // Test sub
    top->sel=1;
    for (int i = 0; i < VALUES_SIZE; i++) { 
        top->a = values[i];
        for (int j = 0; j < VALUES_SIZE; j++) {
            top->b = values[j];
            status_change();
            assert (top->y == values[i] - values[j]);
            assert (top->zero == (top->y == 0));
            printf("a = %d, b = %d, a - b = %d\n", values[i], values[j], top->y);
        }
    }

    // Test not
    top->sel=2;
    for (int i = 0; i < VALUES_SIZE; i++) { 
        top->a = values[i];
        status_change();
        assert (top->y == ~values[i]);
        assert (top->zero == (top->y == 0));
        printf("a = %d, ~a = %d\n", values[i], top->y);
    }

    // Test and
    top->sel=3;
    for (int i = 0; i < VALUES_SIZE; i++) { 
        top->a = values[i];
        for (int j = 0; j < VALUES_SIZE; j++) {
            top->b = values[j];
            status_change();
            assert (top->y == values[i] & values[j]);
            assert (top->zero == (top->y == 0));
            printf("a = %d, b = %d, a & b = %d\n", values[i], values[j], top->y);
        }
    }

    // Test or
    top->sel=4;
    for (int i = 0; i < VALUES_SIZE; i++) { 
        top->a = values[i];
        for (int j = 0; j < VALUES_SIZE; j++) {
            top->b = values[j];
            status_change();
            assert (top->y == values[i] | values[j]);
            assert (top->zero == (top->y == 0));
            printf("a = %d, b = %d, a | b = %d\n", values[i], values[j], top->y);
        }
    }

    // Test xor
    top->sel=5;
    for (int i = 0; i < VALUES_SIZE; i++) { 
        top->a = values[i];
        for (int j = 0; j < VALUES_SIZE; j++) {
            top->b = values[j];
            status_change();
            assert (top->y == values[i] ^ values[j]);
            assert (top->zero == (top->y == 0));
            printf("a = %d, b = %d, a ^ b = %d\n", values[i], values[j], top->y);
        }
    }

    // Test compare
    top->sel=6;
    for (int i = 0; i < VALUES_SIZE; i++) { 
        top->a = values[i];
        for (int j = 0; j < VALUES_SIZE; j++) {
            top->b = values[j];
            status_change();
            assert (top->y == (values[i] < values[j]));
            assert (top->zero == (top->y == 0));
            printf("a = %d, b = %d, a < b = %d\n", values[i], values[j], top->y);
        }
    }
    
    // Test equal
    top->sel=7;
    for (int i = 0; i < VALUES_SIZE; i++) { 
        top->a = values[i];
        for (int j = 0; j < VALUES_SIZE; j++) {
            top->b = values[j];
            status_change();
            assert (top->y == (values[i] == values[j]));
            assert (top->zero == (top->y == 0));
            printf("a = %d, b = %d, a == b = %d\n", values[i], values[j], top->y);
        }
    }

    // =============================
    // ==== End simulation body ====
    // =============================

#ifdef _DO_TRACE
    tfp->close();
    delete tfp;
#endif
    delete top;
    delete contextp;
#ifdef _NVBOARD
    nvboard_quit();
#endif
    printf("Simulation done.\n");
    return 0;
}
