#ifndef __COMMON_H__
#define __COMMON_H__

#ifndef CONFIG_RV64
typedef uint32_t word_t;
#else
typedef uint64_t word_t;
#endif

#endif