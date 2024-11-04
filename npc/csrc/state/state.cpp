#include <state.h>

NPCState npc_state;

void npc_state_init()
{
    npc_state.state = NPC_STOPPED;
    npc_state.halt_pc = 0;
    npc_state.halt_ret = 0;
}

void npc_state_run()
{
    npc_state.state = NPC_RUNNING;
}

void npc_state_exit()
{
    npc_state.state = NPC_EXIT;
}

void npc_state_halt(paddr_t pc, uint32_t ret)
{
    npc_state.state = NPC_STOPPED;
    npc_state.halt_pc = pc;
    npc_state.halt_ret = ret;
}
