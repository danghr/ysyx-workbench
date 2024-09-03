#ifndef __MAIN_H__
#define __MAIN_H__

#include <nvboard.h>
#include TOP_NAME_H_FILE // Defined in npc/Makefile
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "common.h"
#include "misc.h"
#include "regs.h"

typedef enum
{
  NPC_STOPPED = 0,
  NPC_RUNNING = 1,
  NPC_EXIT = 2,
  NPC_ASSERTION_FAIL = 3,
} NPC_Status;

extern "C" void ysyx_24070014_ecall();
extern "C" void ysyx_24070014_ebreak();

#endif
