//
// Created by nils on 11/30/24.
//

#ifndef PREFIX_UTIL_HASH_FUNCTIONS_HPP
#define PREFIX_UTIL_HASH_FUNCTIONS_HPP

#include <functional>
#include <cstdint>

#include "prefix/util/masks.hpp"

namespace prefix::util
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

    assert(r < 32);

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
}


#endif //PREFIX_UTIL_HASH_FUNCTIONS_HPP
