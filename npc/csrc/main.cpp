#include <nvboard.h>
#include TOP_NAME_H_FILE    // Defined in npc/Makefile
#include "verilated.h"
#include "verilated_vcd_c.h"


// Configuration of whether use tracing, sequential logic, or NVBoard
#define _DO_TRACE
#define _SEQUENTIAL_LOGIC
// #define _NVBOARD


// Assertion macro
// Use GOTO to save the waveform when an assertion fails
bool ASSERTION_FAILED = false;
#define ASSERT(cond) \
    if (!(cond)) { \
        printf("Assertion failed at %s:%d\n", __FILE__, __LINE__); \
        ASSERTION_FAILED = true; \
        goto EXIT; \
    }

#define MAX_CYCLES 5
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

// A condition to detect whether the simulation is finished
#define SIMULATE_FINISHED (contextp->time() >= MAX_SIM_TIME || Verilated::gotFinish())
// A macro to simulate until the condition `cond` is met
#define SIMULATE_UNTIL(cond) while (!(cond) && !SIMULATE_FINISHED)
// A macro to simulate until the end of the simulation
#define SIMULATE_LOOP SIMULATE_UNTIL(0)


/***
 * Convert 2's complement representation to unsigned integer, with all bits
 * higher than the given bits set to 0.
 * T must be integer or unsigned integer, and the number of bits must be
 * less than or equal to the number of bits of T.
 * @param number: The number to be converted
 * @param bits: The number of bits of the result in the circuit
 */
template <typename T>
uint64_t convert_2s_complement_to_unsigned(T number, int bits) {
    // Convert to unsigned integer to avoid distraction
    // from signed bit
    uint64_t number_conv = static_cast<uint64_t>(number);

    // Shift left to remove extra bits, then shift back to restore the value
    number_conv <<= (64 - bits);
    number_conv >>= (64 - bits);

    // return the result
    return number_conv;
}

/***
 * Check if the result matches the reference value in the form of the 2's
 * complement representation within the given bits.
 * T must be integer or unsigned integer, and the number of bits must be
 * less than or equal to the number of bits of T.
 * @param result: The result to be checked
 * @param ref: The reference value
 * @param bits: The number of bits of the result in the circuit
 */
template <typename T>
bool check_2s_complement_bits(T result, T ref, int bits) {
    // Convert to unsigned integer to avoid distraction
    // from signed bit
    uint64_t result_conv = convert_2s_complement_to_unsigned<T>(result, bits);
    uint64_t ref_conv = convert_2s_complement_to_unsigned<T>(ref, bits);

    // Return comparison result
    return result_conv == ref_conv;
}

void print_reg() {
    // Map registers signal to an array
    uint32_t *regs = top->top_signal_regfile;

    // Print the value of each register
    for (int i = 0; i < 32; i++) {
        printf("x%d: %d\n", i, regs[i]);
    }
}

uint32_t memory(uint32_t addr) {
    uint32_t insts[] = {
        0x00100093,     // addi x1, x0, 1
        0x00108113,     // addi x2, x1, 1
        0x00a10a13,     // addi x20, x2, 10
        0x00108093,     // addi x1, x1, 1
        0x00108093,     // addi x1, x1, 1
    };
    return insts[addr - 0x80000000];
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

#ifdef _SEQUENTIAL_LOGIC
    reset(3);
#endif

    // =============================
    // === Begin simulation body ===
    // =============================

    int sim_cycle = 0;
    while (sim_cycle++ < MAX_CYCLES) {
        top->top_signal_inst = memory(top->top_signal_pc);    // addi x1, x0, 1
        single_cycle();
        printf("Cycle %d\n", sim_cycle);
        print_reg();
        printf("\n");
    }

    // =============================
    // ==== End simulation body ====
    // =============================

EXIT:
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
    printf("Simulation done.\n");
    return ASSERTION_FAILED ? 1 : 0;
}
