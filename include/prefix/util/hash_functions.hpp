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

    if (r <= 7)
    {
      return r;
    }

    return r - 7;
  }
};
}


#endif //PREFIX_UTIL_HASH_FUNCTIONS_HPP
