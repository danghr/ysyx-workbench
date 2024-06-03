#include <nvboard.h>
#include TOP_NAME_H_FILE    // Defined in npc/Makefile
#include "verilated.h"
#include "verilated_vcd_c.h"


// Configuration of whether use tracing, sequential logic, or NVBoard
#define _DO_TRACE
// #define _SEQUENTIAL_LOGIC
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

    // // **** Test Shifting Register ****
    // for (int i = 0; i < 256; i++) {
    //     int counter = 0;
    //     reset(5);
    //     top->reset = 1;
    //     top->init_val = i;
    //     single_cycle();
    //     printf("Simulate %5d / Output val: %2x / Input val: %2x\n", i, top->out, i);
    //     ASSERT(top->out == i);

    //     SIMULATE_UNTIL(counter >= 1e3) {
    //         counter++;
    //         printf("Simulate %5d-%5d / ", i, counter);
    //         top->reset = 0;
    //         uint8_t prev_value = top->out;
    //         uint8_t now_input_val = rand() % 2;
    //         top->in = now_input_val;
    //         single_cycle();
    //         printf("Output val: %2x / Input bit: %1x\n", top->out, now_input_val);
    //         ASSERT(top->out == ((prev_value >> 1) + (now_input_val << 7)) & 0xff);
    //     }
    // }

    // // **** Test Linear Feedback Shift Register ****
    // for (int i = 0; i < 256; i++) {
    //     int counter = 0;
    //     reset(5);
    //     top->reset = 1;
    //     top->init_val = i;
    //     single_cycle();
    //     printf("Simulate %5d / Output val: %2x / Input val: %2x\n", i, top->out, i);
    //     ASSERT(top->out == i);

    //     SIMULATE_UNTIL(counter >= 1e3) {
    //         counter++;
    //         printf("Simulate %5d-%5d / ", i, counter);
    //         top->reset = 0;
    //         uint8_t prev_value = top->out;
    //         uint8_t leftmost_val = convert_2s_complement_to_unsigned((prev_value >> 4) ^ (prev_value >> 3) ^ (prev_value >> 2) ^ (prev_value), 1);
    //         single_cycle();
    //         printf("Output val: %2x / XOR-ed bit: %1x\n", top->out, leftmost_val);
    //         ASSERT(top->out == ((prev_value >> 1) + (leftmost_val << 7)) & 0xff);
    //     }
    // }

    // **** Test Barrel Shifter ****
    for (int i = 0; i < 1e6; i++) {
        // According to https://stackoverflow.com/questions/7622,
        // Shifting a signed integer to is a undefined behavior,
        // So we manually implement it using an unsigned integer.
        // Logical/Arithmetic shift left: Add zeros to the right.
        // Logical shift right: Add zeros to the left.
        // Arithmetic shift right: Add the highest (sign) bit to the left.

        uint8_t input_val, shift_val, ref_val;
        switch (i)
        {
        case 0:
            input_val = 0x00;
            shift_val = 4;
            break;
        
        case 1:
            input_val = 0x88;
            shift_val = 4;
        
        case 2:
            input_val = 0xFF;
            shift_val = 4;
        
        case 3:
            input_val = 0x88;
            shift_val = 8;
        
        case 4:
            input_val = 0xFF;
            shift_val = 8;
        
        default:
            input_val = rand() % 512;
            shift_val = rand() % 8;
            break;
        }
        
    

        // Test logical shift left
        ref_val = (input_val << shift_val) & 0xff;
        top->din = input_val;
        top->shamt = shift_val;
        top->lr = 1;
        top->al = 0;
        status_change();
        printf("Simulate %5d /    Logical Left  / Input val: %2x / Shift val: %2x / Output val: %2x / Reference val: %2x\n", i, input_val, shift_val, top->out, ref_val);
        ASSERT(top->out == ref_val);

        // Test arithmetic shift left
        ref_val = (input_val << shift_val) & 0xff;
        top->din = input_val;
        top->shamt = shift_val;
        top->lr = 1;
        top->al = 1;
        status_change();
        printf("Simulate %5d / Arithmetic Left  / Input val: %2x / Shift val: %2x / Output val: %2x / Reference val: %2x\n", i, input_val, shift_val, top->out, ref_val);
        ASSERT(top->out == ref_val); 

        // Test logical shift right
        ref_val = (input_val >> shift_val) & 0xff;
        top->din = input_val;
        top->shamt = shift_val;
        top->lr = 0;
        top->al = 0;
        status_change();
        printf("Simulate %5d /    Logical Right /  Input val: %2x / Shift val: %2x / Output val: %2x / Reference val: %2x\n", i, input_val, shift_val, top->out, ref_val);
        ASSERT(top->out == ref_val);

        // Test arithmetic shift right
        ref_val = input_val;
        for (int i = 0; i < shift_val; i++)
            ref_val = (ref_val & 0x80) | (ref_val >> 1);
        top->din = input_val;
        top->shamt = shift_val;
        top->lr = 0;
        top->al = 1;
        status_change();
        printf("Simulate %5d / Arithmetic Right /  Input val: %2x / Shift val: %2x / Output val: %2x / Reference val: %2x\n", i, input_val, shift_val, top->out, ref_val);
        ASSERT(top->out == ref_val);  
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
