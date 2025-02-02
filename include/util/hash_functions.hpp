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

// to suppress -Wpedantic warning, as __int128 is not an iso c++ supported type
__extension__ typedef __int128 int128;

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
  // see Universal hashing and k-wise independent random variables via integer arithmetic without primes by Martin Dietzfelbinger
  constexpr inline static uint8_t fingerprint(uint16_t r)
  {
    return static_cast<uint8_t>((((r >> 8) * 0x12345 + 0x1337) >> 8) % 25);
  }
};

// see Universal hashing and k-wise independent random variables via integer arithmetic without primes by Martin Dietzfelbinger
struct two_wise
{
  static inline int128
    a{(int128)_mm_set_epi64x(0x3eb7c8fca7a155fb, 0xa46fc5be3ef5d14e)};
  static inline int128
    b{(int128)_mm_set_epi64x(0xa2a5c4739022a919, 0x9eecc1b094a0c649)};

  inline static uint64_t fp(uint64_t key)
  {

    int128 key_128{(int128)_mm_set_epi64x(0x0, key)};


    return (uint64_t)(((a * key_128 + b)) >> 64);
  }
};

template<uint64_t size, uint64_t no_hashes>
struct bloom_hash_function
{
  constexpr inline static std::array<std::size_t, no_hashes> hash(std::pair<std::size_t, uint8_t> fp)
  {
    uint64_t key{fp.first << 8 | static_cast<uint64_t>(fp.second)};
    std::array<std::size_t, no_hashes> to_return{};
    auto current_acc{key};
    for (auto& position : to_return)
    {
      current_acc = two_wise::fp(current_acc);
      position = current_acc % size;
    }

    return to_return;

  }

  constexpr inline static std::array<std::size_t, no_hashes> hash(uint64_t fp)
  {
    uint64_t key{fp};
    std::array<std::size_t, no_hashes> to_return{};
    auto current_acc{key};
    for (auto& position : to_return)
    {
      current_acc = two_wise::fp(current_acc);
      position = current_acc % size;
    }

    return to_return;

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


    to_return.first = two_wise::fp(key) % size;
    to_return.second = static_cast<uint16_t>(key & UINT16_MAX);

    return to_return;
  }
};

}// namespace util
#endif // UTIL_HASH_FUNCTIONS_HPP
