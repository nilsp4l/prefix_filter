//
// Created by nils on 12/22/24.
//

#ifndef INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP
#define INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP

#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <optional>
#include "prefix/interfaces/i_bin.hpp"
#include "util/masks.hpp"

namespace prefix::adapted
{

/// this implementation has a different kind of header
/// because if storing keys at most once allows just low cycle simd instructions to do the search
/// which is more efficient, than looking inside the header first.
/// The header stores the size in the 5 least significant bits of the first byte of data_. The first most significant bit
/// marks whether the pd is overflowed
/// This gives a header size of one byte. Hence, everything after that is a key. Giving the ability to store 31 bytes
class bin : public interfaces::i_bin
{
public:
  bin();

  ~bin() override;

  uint8_t& operator[](const uint8_t index) const override;

  [[nodiscard]] bool query(uint8_t r) const override;

  std::optional<uint8_t> insert(uint8_t r) override;

  [[nodiscard]] uint8_t size() const override;

  [[nodiscard]] bool overflowed() const;

  void set_overflowed() const;

private:
  void increase_size();

  uint8_t* data_;
};
} // namespace prefix::adapted
#endif // INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP
