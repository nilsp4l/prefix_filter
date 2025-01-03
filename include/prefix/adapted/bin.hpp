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
  bin()
  {
    data_ = new uint8_t[32]();
  }

  ~bin() override
  {
    delete[] data_;
  }

  uint8_t& operator[](const uint8_t index) const override
  {
    return data_[1 + index];
  }

  [[nodiscard]] bool query(uint8_t r) const override
  {
    if (!size())
    {
      return false;
    }

    // loadu may be used, as we never cross cache lines
    auto pd_reg{_mm256_loadu_si256(reinterpret_cast<__m256i*>(data_))};

    // ignore the lowest byte

    auto cmp_result{_mm256_cmpeq_epi8(pd_reg, _mm256_set1_epi8(static_cast<int8_t>(r)))};
    return ((_mm256_movemask_epi8(cmp_result) & (util::bit_mask_right_rt<int>::value(size())) << 1) != 0);
  }

  std::optional<uint8_t> insert(uint8_t r) override
  {
    // Because of the overflowed procedure:
    // query code is copied here and not called, because the _mm256_set1_epi8 instruction is sequential
    // doing that twice would be wasteful. Furthermore, the loaded register is still relevant

    auto pd_reg{_mm256_loadu_si256(reinterpret_cast<__m256i*>(data_))};

    auto r_reg{_mm256_set1_epi8(static_cast<int8_t>(r))};
    auto cmp_result{_mm256_cmpeq_epi8(pd_reg, r_reg)};
    auto valid_elements_mask{util::bit_mask_right_rt<int>::value(size()) << 1};

    if ((_mm256_movemask_epi8(cmp_result) & valid_elements_mask) != 0)
    {
      return std::nullopt;
    }

    if (size() == 31)
    {
      set_overflowed();
    }

    if (!overflowed())
    {
      data_[size() + 1] = r;
      increase_size();
      return std::nullopt;
    }

    // static cast to uint is necessary to not shift arithmetically
    auto
      r_is_max{static_cast<uint32_t>(_mm256_movemask_epi8(_mm256_cmpeq_epi8(r_reg, _mm256_max_epu8(r_reg, pd_reg))))};

    // overflowed procedure
    auto max_element_positions{(~r_is_max) & static_cast<uint32_t>(valid_elements_mask)};

    if (max_element_positions == 0)
    {
      return r;
    }
    max_element_positions >>= 1;
    uint8_t max_index{0};
    uint8_t max_value{0};
    uint8_t current_index{0};
    while (max_element_positions)
    {
      if ((max_element_positions & util::bit_mask_position<int, 31>::value) && data_[1 + current_index] > max_value)
      {
        max_value = data_[1 + current_index];
        max_index = current_index;
      }
      max_element_positions >>= 1;
      ++current_index;
    }
    uint8_t max{data_[1 + max_index]};
    data_[1 + max_index] = r;
    return max;
  }

  [[nodiscard]] uint8_t size() const override
  {
    return (*data_ & util::bit_mask_right<uint8_t, 5>::value);
  }

  [[nodiscard]] bool overflowed() const
  {
    return util::bit_mask_position<uint8_t, 0>::value & *data_;
  }

  void set_overflowed() const
  {
    (*data_) |= 0x80;
  }

private:
  void increase_size()
  {
    // we may ignore that the most significant bit is set, as we will never reach the point where incrementing will change that
    // as we are just manipulating the least significant 5 bits and not more here
    ++(*data_);
  }

  uint8_t* data_;
};
} // namespace prefix::adapted
#endif // INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP
