//
// Created by nils on 11/16/24.
//

#ifndef PREFIX_UTIL_MASKS_H
#define PREFIX_UTIL_MASKS_H

#include <bit>
#include <cstdint>

namespace prefix::util
{
// creates a bitmask with one singular bit set at position counting from left starting at 0
// for example: bit_mask<uint8_t, 2>::value == 0b0010 0000
template<typename numberT, uint8_t position>
requires std::is_integral_v<numberT>
struct bit_mask_position
{
  static constexpr numberT
    value{static_cast<numberT>(static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1)))};
};

template<typename numberT>
requires std::is_integral_v<numberT>
struct bit_mask_position_rt
{
  static constexpr numberT value(uint8_t position)
  {
    return static_cast<numberT>(static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1)));
  }
};

// creates a bitmask with all bits set up to position counting from left starting at 0
// for example: bit_mask<uint8_t, 2>::value == 0b1110 0000
template<typename numberT, uint8_t position>
requires std::is_integral_v<numberT>
struct bit_mask_left
{
  static constexpr numberT
    value{static_cast<numberT>(~((static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1))) - 1))};
};

template<typename numberT>
requires std::is_integral_v<numberT>
struct bit_mask_left_rt
{
  static constexpr numberT value(uint8_t position)
  {
    return static_cast<numberT>(~((static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1))) - 1));
  }
};

} // namespace prefix::util

#endif // PREFIX_UTIL_MASKS_H
