//
// Created by nils on 11/30/24.
//

#ifndef UTIL_HASH_FUNCTIONS_HPP
#define UTIL_HASH_FUNCTIONS_HPP

#include <functional>
#include <cstdint>
#include <tuple>
#include <random>

#include "masks.hpp"

namespace util
{

// we are interested in having the biggest element stored in the last non-empty list,
// hence we work the most significant bits to determine the index
template<uint8_t Q>
struct most_significant_based_fp
{
  constexpr inline static uint8_t fingerprint(uint16_t)
  {
    return 0;
  };
};

template<>
struct most_significant_based_fp<25>
{
  constexpr inline static uint8_t fingerprint(uint16_t r)
  {
    r >>= 8;
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

struct carter_wegman_hash
{
  static inline __int128 a{(__int128)_mm_set_epi64x(0x3eb7c8fca7a155fb, 0xa46fc5be3ef5d14e)};
  static inline __int128 b{(__int128)_mm_set_epi64x(0xa2a5c4739022a919, 0x9eecc1b094a0c649)};
  static inline __int128 p{(__int128)_mm_set_epi64x(0x7ba1d7f87, 0x5d9f6a5582f3a125)};

  inline static uint64_t fp(uint64_t key)
  {

    __int128 key_128{(__int128)_mm_set_epi64x(0x0, key)};


    return (uint64_t)(((a * key_128 + b) % p) >> 64);
  }
};

template<uint64_t size>
struct bloom_hash_function
{
  constexpr inline static std::array<std::size_t, 3> hash(std::pair<std::size_t, uint8_t> fp)
  {
    uint64_t key{fp.first << 8 | static_cast<uint64_t>(fp.second)};
    auto foo{size};
    std::array<std::size_t, 3> to_return{};
    auto current_acc{key};
    for (auto& position : to_return)
    {
      current_acc = carter_wegman_hash::fp(current_acc);
      position = current_acc % size;
    }

    return to_return;

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

  inline static std::pair<std::size_t, uint16_t> fp(uint64_t key)
  {
    std::pair<std::size_t, uint16_t> to_return;


    to_return.first = carter_wegman_hash::fp(key) % size;
    to_return.second = static_cast<uint16_t>(key & UINT16_MAX);

    return to_return;
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
