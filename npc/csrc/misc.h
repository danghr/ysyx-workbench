#ifndef __MISC_H__
#define __MISC_H__

#include <stdint.h>
#include "common.h"


template <typename T>
bool check_2s_complement_bits(T result, T ref, int bits);
template <typename T>
word_t convert_2s_complement_to_unsigned(T number, int bits);

#endif
