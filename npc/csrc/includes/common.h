#ifndef __COMMON_H__
#define __COMMON_H__

#include <cstdint>
#include <cinttypes>
#include "config.h"
#include "macro.h"
#include "utils.h"

typedef uint32_t word_t;
typedef int32_t sword_t;
#define FMT_WORD MUXDEF(CONFIG_ISA64, "0x%016" PRIx64, "0x%08" PRIx32)

typedef uint32_t paddr_t;
#define FMT_PADDR MUXDEF(PMEM64, "0x%016" PRIx64, "0x%08" PRIx32)

typedef uint8_t byte;

#endif