#include <nvboard.h>
#include TOP_NAME_H_FILE // Defined in npc/Makefile
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <regs.h>
#include <memory/paddr.h>
#include <utils.h>
#include <state.h>
#include <debug.h>

// Configuration of whether use tracing, sequential logic, or NVBoard
#define _DO_TRACE
#define _SEQUENTIAL_LOGIC
// #define _NVBOARD

void init_monitor(int, char *[]);

extern NPCState npc_state;
static TOP_NAME *top; // Defined in npc/Makefile
VerilatedContext *contextp;
VerilatedVcdC *tfp;

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

void status_change()
{
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
void single_cycle()
{
	top->clk = 1;
	status_change();
	top->clk = 0;
	status_change();
}

void reset(int n)
{
	top->reset = 1;
	while (n-- > 0)
		single_cycle();
	top->reset = 0;
}
#endif

// A condition to detect whether the simulation is finished
#define SIMULATE_FINISHED (contextp->time() >= MAX_SIM_TIME || Verilated::gotFinish())
// A macro to simulate until the condition `cond` is met
#define SIMULATE_UNTIL(cond) while (!(cond) && !SIMULATE_FINISHED)
// A macro to simulate until the end of the simulation
#define SIMULATE_LOOP SIMULATE_UNTIL(0)

int main(int argc, char **argv)
{
	// ========================
	// === Simulation Setup ===
	// ========================
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

	// ==================================
	// === Environment Initialization ===
	// ==================================

	npc_state_init();
	init_monitor(argc, argv);

	// Restart by setting the initial program counter
  	top->top_signal_pc = RESET_VECTOR;

#ifdef _SEQUENTIAL_LOGIC
	reset(20);
#endif

	// =============================
	// === Begin simulation body ===
	// =============================

SIMULATE_BEGIN:
	npc_state_run();
	printf("NPC now running\n");
	SIMULATE_UNTIL(npc_state.state != NPC_RUNNING || SIMULATE_FINISHED);
	{
		top->top_signal_inst = paddr_read(top->top_signal_pc); // addi x1, x0, 1
		printf("Instruction: %08x\n", top->top_signal_inst);
		single_cycle();
		bool reg_success = false;
		assert(isa_reg_str2val(top, "x0", &reg_success) == 0);
		assert(reg_success = true);
	}
	isa_reg_display(top);
	printf("\n");

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
	if (npc_state.state == NPC_EXIT)
	{
		printf("Simulation finished successfully.\n");
		return_status = 0;
	}
	else if (npc_state.state == NPC_STOPPED)
	{
		printf("Simulation stopped.\n");
		return_status = 0;
	}
	else if (npc_state.state == NPC_ASSERTION_FAIL)
	{
		printf("Simulation failed due to assertion failure.\n");
		return_status = 1;
	}
	else
	{
		printf("Simulation failed due to unknown reason. NEMU_STATE: %d\n", npc_state.state);
		return_status = 1;
	}
	return return_status;
}

void assert_fail_msg() {
  isa_reg_display(top);
//   statistic();
}

// Miscaellaneous functions
// ecall and ebreak
extern "C" void ysyx_24070014_ecall()
{
	printf("Calling ecall\n");
	printf("Not implemented\n");
	Assert(0, "ecall not implemented");
}

extern "C" void ysyx_24070014_ebreak()
{
	printf("Calling ebreak\n");
	npc_state_exit();
}
