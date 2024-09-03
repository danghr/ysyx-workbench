#ifndef __MISC_H__
#define __MISC_H__

#include <stdint.h>
#include "common.h"


// Assertion macro
#define ASSERT(cond) \
    if (!(cond)) { \
        printf("Assertion failed at %s:%d\n", __FILE__, __LINE__); \
        npc_status = NPC_ASSERTION_FAIL; \
    }


template <typename T>
bool check_2s_complement_bits(T result, T ref, int bits);
template <typename T>
word_t convert_2s_complement_to_unsigned(T number, int bits);

#endif
