#include "main.h"

// Configuration of whether use tracing, sequential logic, or NVBoard
#define _DO_TRACE
#define _SEQUENTIAL_LOGIC
// #define _NVBOARD


NPC_Status npc_status = NPC_STOPPED; 
static TOP_NAME *top;   // Defined in npc/Makefile
VerilatedContext* contextp;
VerilatedVcdC* tfp;


// Define a maximum simulation time and cycles
#define MAX_CYCLES 1e9
#ifdef _SEQUENTIAL_LOGIC
const int MAX_SIM_TIME = (MAX_CYCLES) * 2;
#else
const int MAX_SIM_TIME = (MAX_CYCLES);
#endif

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

// A condition to detect whether the simulation is finished
#define SIMULATE_FINISHED (contextp->time() >= MAX_SIM_TIME || Verilated::gotFinish())
// A macro to simulate until the condition `cond` is met
#define SIMULATE_UNTIL(cond) while (!(cond) && !SIMULATE_FINISHED)
// A macro to simulate until the end of the simulation
#define SIMULATE_LOOP SIMULATE_UNTIL(0)


uint32_t memory(uint32_t addr) {
    uint32_t insts[] = {
        0x00100093,     // addi x1, x0, 1
        0x00108113,     // addi x2, x1, 1
        0x00a10a13,     // addi x20, x2, 10
        0x00108093,     // addi x1, x1, 1
        0x00108093,     // addi x1, x1, 1
        0x00100073,     // ebreak
    };
    return insts[(addr - 0x80000000) / 4];
}


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

    npc_status = NPC_RUNNING;

#ifdef _SEQUENTIAL_LOGIC
    reset(3);
#endif

    // =============================
    // === Begin simulation body ===
    // =============================

SIMULATE_BEGIN:
    SIMULATE_UNTIL(npc_status != NPC_RUNNING) {
        top->top_signal_inst = memory(top->top_signal_pc);    // addi x1, x0, 1
        single_cycle();
        bool reg_success = false;
        ASSERT(isa_reg_str2val(top, "x0", &reg_success) == 0);
        ASSERT(reg_success = true);
    }

    // =============================
    // ==== End simulation body ====
    // =============================

SIMULATE_END:
    // An extra cycle to dump the trace of the last signal
    status_change();
#ifdef _DO_TRACE
    tfp->close();
    delete tfp;
#endif
    delete top;
    delete contextp;
#ifdef _NVBOARD
    nvboard_quit();
#endif

    int return_status = 1;
    if (npc_status == NPC_EXIT) {
        printf("Simulation finished successfully.\n");
        return_status = 0;
    } else if (npc_status == NPC_ASSERTION_FAIL) {
        printf("Simulation failed due to assertion failure.\n");
        return_status = 1;
    } else {
        printf("Simulation failed due to unknown reason.\n");
        return_status = 1;
    }
    return return_status;
}


// Miscaellaneous functions
// ecall and ebreak
extern "C" void ysyx_24070014_ecall() {
    printf("Calling ecall\n");
    printf("Not implemented\n");
    ASSERT(0);
    return ;
}

extern "C" void ysyx_24070014_ebreak() {
    printf("Calling ebreak\n");
    npc_status = NPC_EXIT;
    return ;
}
