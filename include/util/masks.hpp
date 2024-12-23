//
// Created by nils on 11/16/24.
//

#ifndef UTIL_MASKS_HPP
#define UTIL_MASKS_HPP

#include <bit>
#include <cstdint>
#include <immintrin.h>


namespace util
{
  // creates a bitmask with one singular bit set at position counting from left starting at 0
  // for example: bit_mask<uint8_t, 2>::value == 0b0010 0000
  template <typename numberT, uint8_t position>
    requires std::is_integral_v<numberT>
  struct bit_mask_position
  {
    static constexpr numberT
        value{static_cast<numberT>(static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1)))};
  };

  template <typename numberT>
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
  template <typename numberT, uint8_t position>
    requires std::is_integral_v<numberT>
  struct bit_mask_left
  {
    static constexpr numberT
        value{static_cast<numberT>(~((static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1))) - 1))};
  };

  template <typename numberT>
    requires std::is_integral_v<numberT>
  struct bit_mask_left_rt
  {
    static constexpr numberT value(uint8_t position)
    {
      return static_cast<numberT>(~((static_cast<numberT>(1) << ((sizeof(numberT) << 3) - (position + 1))) - 1));
    }
  };

  template <uint8_t position>
  struct _m256i_bit_mask_left
  {
    static const __m256i value()
    {
      return _mm256_srli_si256(_mm256_set1_epi64x(-1), position);
    }
  };


  struct _mm256i_bit_mask_no_header
  {
    static const __m256i value()
    {
      return _mm256_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0);
    }
  };

} // namespace util

#endif // UTIL_MASKS_HPP
