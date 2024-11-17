#ifndef __STATE_H__
#define __STATE_H__

#include <common.h>

// ----------- state -----------

typedef enum
{
  NPC_STOPPED = 0,
  NPC_RUNNING = 1,
  NPC_EXIT = 2,
  NPC_ASSERTION_FAIL = 3,
} NPC_Status;

typedef struct {
  int state;
  paddr_t halt_pc;
  uint32_t halt_ret;
} NPCState;

void npc_state_init();
void npc_state_run();
void npc_state_exit();
void npc_state_halt(paddr_t pc, uint32_t ret);

#endif