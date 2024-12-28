//
// Created by nils on 12/22/24.
//

#ifndef INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP
#define INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP

#include <cstdint>
#include <immintrin.h>
#include <optional>
#include "prefix/interfaces/i_pocket_dictionary.hpp"
#include "util/masks.hpp"

namespace prefix::adapted
{

/// this implementation has a different kind of header
/// because if storing keys at most once allows just low cycle simd instructions to do the search
/// which is more efficient, than looking inside the header first.
/// The header stores the size in the first 5 bits of the first byte of data_. The following 3 bits indicate
/// whether the pd contains a 0, the pd is overflowed and ?nothing? respectively
/// This gives a header size of one byte. Hence, everything after that is a key. Giving the ability to store 31 bytes
class pocket_dictionary
{
public:
  pocket_dictionary()
  {
    data_ = new uint8_t[32]();
  }

  ~pocket_dictionary()
  {
    delete[] data_;
  }

  uint8_t& operator[](const uint8_t index) const
  {
    return data_[1 + index];
  }

  [[nodiscard]] bool query(uint8_t r) const
  {

    if (r == 0)
    {
      return contains_zero();
    }
    // loadu may be used, as we never cross cache lines
    auto avx_reg{_mm256_loadu_si256(reinterpret_cast<__m256i*>(data_))};

    // ignore the lowest byte
    avx_reg = _mm256_and_si256(avx_reg, util::_mm256i_bit_mask_no_header::value());

    auto cmp_result{_mm256_cmpeq_epi8(avx_reg, _mm256_set1_epi8(r))};
    return (_mm256_movemask_epi8(cmp_result) != 0);
  }

  std::optional<uint8_t> insert(uint8_t r)
  {
    // Because of the overflowed procedure:
    // query code is copied here and not called, because the _mm256_set1_epi8 instruction is sequential
    // doing that twice would be wasteful. Furthermore, the loaded register is still relevant

    if (r == 0 && !contains_zero())
    {
      insert_zero();
      increase_size();
      return std::nullopt;
    }

    // 0 already inserted
    if (r == 0)
    {
      return std::nullopt;
    }

    auto avx_reg{_mm256_loadu_si256(reinterpret_cast<__m256i*>(data_))};

    // ignore lowest byte
    avx_reg = _mm256_and_si256(avx_reg, util::_mm256i_bit_mask_no_header::value());
    auto r_reg{_mm256_set1_epi8(r)};
    auto cmp_result{_mm256_cmpeq_epi8(avx_reg, r_reg)};

    if (_mm256_movemask_epi8(cmp_result) != 0)
    {
      return std::nullopt;
    }

    if (!overflowed())
    {
      data_[size() + 1] = r;
      increase_size();
      return std::nullopt;
    }

    // overflowed procedure
    auto max_element_positions{_mm256_movemask_epi8(_mm256_max_epi8(r_reg, avx_reg))};

    if (max_element_positions == 0)
    {
      return r;
    }

    uint8_t max_index{0};
    uint8_t max_value{0};
    uint8_t current_index{0};
    while (max_element_positions)
    {

      if ((max_element_positions & util::bit_mask_position<int, 0>::value) && data_[1 + current_index] > max_value)
      {
        max_value = data_[1 + current_index];
        max_index = current_index;
      }
      max_element_positions <<= 1;
      ++current_index;
    }
    uint8_t max{data_[1 + max_index]};
    data_[1 + max_index] = r;
    return max;
  }

  [[nodiscard]] uint8_t size() const
  {
    return static_cast<uint8_t>((*data_ & util::bit_mask_left<uint8_t, 4>::value) >> 3);
  }

  [[nodiscard]] bool overflowed() const
  {
    return util::bit_mask_position<uint8_t, 5>::value & *data_;
  }

private:
  void increase_size()
  {
    *data_ = static_cast<uint8_t>((((*data_ & util::bit_mask_left<uint8_t, 4>::value) >> 3) + 1) << 3)
      | (*data_ & ~util::bit_mask_left<uint8_t, 4>::value);
  }

  bool contains_zero() const
  {
    return util::bit_mask_position<uint8_t, 6>::value & *data_;
  }

  void insert_zero()
  {
    *data_ |= util::bit_mask_position<uint8_t, 6>::value;
  }

  uint8_t* data_;
};
} // namespace prefix::adapted
#endif // INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP
