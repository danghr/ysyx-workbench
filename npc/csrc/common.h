#ifndef __COMMON_H__
#define __COMMON_H__

#ifndef CONFIG_RV64
typedef uint32_t word_t;
#else
typedef uint64_t word_t;
#endif

// Assertion macro
#define ASSERT(cond) \
    if (!(cond)) { \
        printf("Assertion failed at %s:%d\n", __FILE__, __LINE__); \
        npc_status = NPC_ASSERTION_FAIL; \
    }

#endif