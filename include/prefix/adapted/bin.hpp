//
// Created by nils on 12/22/24.
//

#ifndef INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP
#define INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP

#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <optional>
#include "util/masks.hpp"

namespace prefix::adapted
{

/// this implementation has a different kind of header
/// because if storing keys at most once allows just low cycle simd instructions to do the search
/// which is more efficient, than looking inside the header first.
/// The header stores the size in the 5 least significant bits of the first byte of data_. The first most significant bit
/// marks whether the pd is overflowed
/// This gives a header size of one byte. Hence, everything after that is a key. Giving the ability to store 31 bytes
class bin
{
public:

  static constexpr std::size_t maximum_size{31};

  static bool query(uint8_t r, uint8_t* data);

  static std::optional<uint8_t> insert(uint8_t r, uint8_t* data);

  static uint8_t size(uint8_t* data);

  static bool overflowed(uint8_t* data);

  static void set_overflowed(uint8_t* data);

  static bool greater_than_max(uint8_t fp, uint8_t* data);

private:
  static void increase_size(uint8_t* data);

};
} // namespace prefix::adapted
#endif // INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP
