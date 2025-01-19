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
  constexpr inline static uint8_t fingerprint(uint8_t)
  {
    return 0;
  };
};

template<>
struct most_significant_based_fp<25>
{
  constexpr inline static uint8_t fingerprint(uint8_t r)
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

template<uint32_t size>
struct bloom_hash_function
{
  constexpr inline static std::array<std::size_t, 3> hash(std::pair<std::size_t, uint8_t> fp)
  {
    uint64_t new_fingerprint{((static_cast<uint64_t>(fp.first) << 16) >> 16) | static_cast<uint64_t>(fp.second)};
    return {hash_1(new_fingerprint) & size, hash_2(new_fingerprint) % size,
            (hash_1(new_fingerprint) ^ hash_2(new_fingerprint)) % size};
  }

private:
  constexpr inline static std::size_t hash_1(uint64_t key)
  {
    return key / 11;
  }

  constexpr inline static std::size_t hash_2(uint64_t key)
  {
    return static_cast<std::size_t>(key);
  }
};

template<typename key_t, std::size_t size>
struct prefix_fingerprint
{

};

template<std::size_t size>
struct prefix_fingerprint<uint64_t, size>
{
  constexpr inline static std::pair<std::size_t, uint8_t> fp(uint64_t key)
  {
    std::pair<std::size_t, uint8_t> to_return;

    to_return.first = key % size;
    to_return.second = key & UINT8_MAX;

    return to_return;
  }
};

struct cuckoo_fingerprint
{
  constexpr inline static uint16_t fp(uint64_t key)
  {
    return key & (UINT16_MAX - 1);
  }
};

template<std::size_t no_buckets>
struct cuckoo_hash
{
  constexpr inline static uint64_t hash(uint64_t key)
  {
    return key % no_buckets;
  }
};

}// namespace util
#endif // UTIL_HASH_FUNCTIONS_HPP
