//
// Created by nils on 11/16/24.
//

#ifndef UTIL_MASKS_HPP
#define UTIL_MASKS_HPP

#include <bit>
#include <cstdint>
#include <immintrin.h>
#include <emmintrin.h>

namespace util
{

const __m128i reg_128_all_set{_mm_set1_epi64x(0xffffffffffffffff)};

// creates a bitmask with one singular bit set at position counting from left starting at 0
// for example: bit_mask<uint8_t, 2>::value == 0b0010 0000
template<typename numberT, uint8_t position>
struct bit_mask_position
{
  static inline constexpr numberT
    value{static_cast<numberT>(static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1)))};
};

template<typename numberT>
struct bit_mask_position_rt
{
  static inline constexpr numberT value(uint8_t position)
  {
    return static_cast<numberT>(static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1)));
  }
};

// creates a bitmask with all bits set up to position counting from left starting at 0
// for example: bit_mask<uint8_t, 2>::value == 0b1110 0000
template<typename numberT, uint8_t position>
struct bit_mask_left
{
  static inline constexpr numberT
    value{static_cast<numberT>(~((static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1))) - 1))};
};

template<typename numberT>
struct bit_mask_left_rt
{
  static inline constexpr numberT value(uint8_t position)
  {
    return static_cast<numberT>(~((static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1))) - 1));
  }
};

// creates a bitmask with all bits set up to position counting from right starting at 0
// for example: bit_mask<uint8_t, 2>::value == 0b0000 0011
template<typename numberT, uint8_t position>
struct bit_mask_right
{
  static inline constexpr numberT
    value{static_cast<numberT>((static_cast<numberT>(1) << position) - 1)};
};

template<typename numberT>
struct bit_mask_right_rt
{
  static inline constexpr numberT value(uint8_t position)
  {
    return static_cast<numberT>(static_cast<numberT>((static_cast<numberT>(1) << position) - 1));
  }
};

template<uint8_t offset>
struct reg_128_bit_mask_left
{
  static inline constexpr __m128i value()
  {
    return _mm_slli_si128(reg_128_all_set, 16 - offset);
  }
};

template<uint8_t offset>
struct reg_128_bit_mask_right
{
  static inline constexpr __m128i value()
  {
    return _mm_srli_si128(reg_128_all_set, 16 - offset);
  }
};

struct reg_128_bit_mask_right_rt
{
  static inline constexpr __m128i value(uint8_t offset)
  {
    switch (offset)
    {
    case 0:
      return reg_128_bit_mask_right<0>::value();
    case 1:
      return reg_128_bit_mask_right<1>::value();
    case 2:
      return reg_128_bit_mask_right<2>::value();
    case 3:
      return reg_128_bit_mask_right<3>::value();
    case 4:
      return reg_128_bit_mask_right<4>::value();
    case 5:
      return reg_128_bit_mask_right<5>::value();
    case 6:
      return reg_128_bit_mask_right<6>::value();
    case 7:
      return reg_128_bit_mask_right<7>::value();
    case 8:
      return reg_128_bit_mask_right<8>::value();
    case 9:
      return reg_128_bit_mask_right<9>::value();
    case 10:
      return reg_128_bit_mask_right<10>::value();
    case 11:
      return reg_128_bit_mask_right<11>::value();
    case 12:
      return reg_128_bit_mask_right<12>::value();
    case 13:
      return reg_128_bit_mask_right<13>::value();
    case 14:
      return reg_128_bit_mask_right<14>::value();
    case 15:
      return reg_128_bit_mask_right<15>::value();
    case 16:
      // always return a 0 register at this point
      [[fallthrough]];
    default:
      return _mm_setzero_si128();
    }
  }
};

struct reg_128_bit_mask_left_rt
{
  static inline constexpr __m128i value(uint8_t offset)
  {
    switch (offset)
    {
    case 0:
      return reg_128_bit_mask_left<0>::value();
    case 1:
      return reg_128_bit_mask_left<1>::value();
    case 2:
      return reg_128_bit_mask_left<2>::value();
    case 3:
      return reg_128_bit_mask_left<3>::value();
    case 4:
      return reg_128_bit_mask_left<4>::value();
    case 5:
      return reg_128_bit_mask_left<5>::value();
    case 6:
      return reg_128_bit_mask_left<6>::value();
    case 7:
      return reg_128_bit_mask_left<7>::value();
    case 8:
      return reg_128_bit_mask_left<8>::value();
    case 9:
      return reg_128_bit_mask_left<9>::value();
    case 10:
      return reg_128_bit_mask_left<10>::value();
    case 11:
      return reg_128_bit_mask_left<11>::value();
    case 12:
      return reg_128_bit_mask_left<12>::value();
    case 13:
      return reg_128_bit_mask_left<13>::value();
    case 14:
      return reg_128_bit_mask_left<14>::value();
    case 15:
      return reg_128_bit_mask_left<15>::value();
    case 16:
      // always return a 0 register at this point
      [[fallthrough]];
    default:
      return _mm_setzero_si128();
    }
  }
};

} // namespace util

#endif // UTIL_MASKS_HPP
