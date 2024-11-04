#include <common.h>

/***
 * Convert 2's complement representation to unsigned integer, with all bits
 * higher than the given bits set to 0.
 * T must be integer or unsigned integer, and the number of bits must be
 * less than or equal to the number of bits of T.
 * @param number: The number to be converted
 * @param bits: The number of bits of the result in the circuit
 */
template <typename T>
word_t convert_2s_complement_to_unsigned(T number, int bits)
{
  // Convert to unsigned integer to avoid distraction
  // from signed bit
  uint64_t number_conv = static_cast<uint64_t>(number);

  // Shift left to remove extra bits, then shift back to restore the value
  number_conv <<= (64 - bits);
  number_conv >>= (64 - bits);

  // return the result
  return (word_t)number_conv;
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
bool check_2s_complement_bits(T result, T ref, int bits)
{
  // Convert to unsigned integer to avoid distraction
  // from signed bit
  uint64_t result_conv = convert_2s_complement_to_unsigned<T>(result, bits);
  uint64_t ref_conv = convert_2s_complement_to_unsigned<T>(ref, bits);

  // Return comparison result
  return result_conv == ref_conv;
}
