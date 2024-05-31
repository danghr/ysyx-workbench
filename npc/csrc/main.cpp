#include <nvboard.h>
#include TOP_NAME_H_FILE    // Defined in npc/Makefile
#include "verilated.h"
#include "verilated_vcd_c.h"


// Configuration of whether use tracing, sequential logic, or NVBoard
#define _DO_TRACE
#define _SEQUENTIAL_LOGIC
// #define _NVBOARD


#define MAX_CYCLES 1e7
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

    // Test Shifting Register
    for (int i = 0; i < 100; i++) {
        int counter = 0;
        reset(5);
        uint8_t init_input_val = rand();
        top->reset = 1;
        top->init_val = init_input_val;
        single_cycle();
        printf("Output val: %2x / Input val: %2x\n", top->out, init_input_val);
        assert(top->out == init_input_val);

        SIMULATE_UNTIL(counter >= 1e5) {
            counter++;
            top->reset = 0;
            uint8_t prev_value = top->out;
            uint8_t now_input_val = rand() % 2;
            top->in = now_input_val;
            single_cycle();
            printf("Output val: %2x / Input bit: %1x\n", top->out, now_input_val);
            assert(top->out == ((prev_value >> 1) + (now_input_val << 7)) & 0xff);
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
