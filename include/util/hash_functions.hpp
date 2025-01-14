//
// Created by nils on 11/30/24.
//

#ifndef UTIL_HASH_FUNCTIONS_HPP
#define UTIL_HASH_FUNCTIONS_HPP

#include <functional>
#include <cstdint>
#include <tuple>

#include "masks.hpp"

namespace util
{

// we are interested in having the biggest element stored in the last non-empty list,
// hence we work the most significant bits to determine the index
template<uint8_t Q>
struct most_significant_based_fp
{
  constexpr static uint8_t fingerprint(uint8_t)
  {
    return 0;
  };
};

template<>
struct most_significant_based_fp<25>
{
  constexpr static uint8_t fingerprint(uint8_t r)
  {
    r = r & bit_mask_left<uint8_t, 5>::value;
    r >>= 3;

    switch (r)
    {
    case 0 :
      [[fallthrough]];
    case 1:
      return 0;
    case 2:
      [[fallthrough]];
    case 3:
      return 1;
    case 4:
      [[fallthrough]];
    case 5:
      return 2;
    case 6:
      [[fallthrough]];
    case 7:
      return 3;
    case 8:
      [[fallthrough]];
    case 9:
      return 4;
    case 10:
      [[fallthrough]];
    case 11:
      return 5;
    case 12:
      return 6;
    default:
      return r - 7;
    }
  }
};

template<typename key_t, uint32_t size>
requires (std::popcount(size) == 1)
struct bloom_hash_function
{

};
template<>
struct bloom_hash_function<uint16_t, 4096>
{
  constexpr static std::array<uint32_t, 3> hash(uint16_t key)
  {
    return {
      static_cast<uint32_t>(key) & (256 - 1), static_cast<uint32_t>(key) & (1024 - 1),
      static_cast<uint32_t>(key >> 6) & (4096 - 1)
    };
  }
};

template<typename key_t, std::size_t size>
struct prefix_fingerprint
{

};

template<>
struct prefix_fingerprint<uint64_t, 1000000>
{
  constexpr static std::pair<std::size_t, uint8_t> fp(uint64_t key)
  {
    std::pair<std::size_t, uint8_t> to_return;

    to_return.first = key % 1000000;
    auto current_bit{bit_mask_right<uint8_t, 0>::value};
    while (key)
    {
      to_return.second += current_bit & static_cast<uint8_t>(key);
      key >>= 8;
      current_bit <<= 1;
    }

    return to_return;
  }
};

}// namespace util
#endif // UTIL_HASH_FUNCTIONS_HPP
