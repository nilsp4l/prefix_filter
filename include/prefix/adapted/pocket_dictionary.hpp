//
// Created by nils on 12/22/24.
//

#ifndef INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP
#define INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP

#include <cstdint>
#include <immintrin.h>
#include "prefix/interfaces/i_pocket_dictionary.hpp"

namespace prefix::adapted
{

/// this implementation has a different kind of header
/// because if storing keys at most once allows just low cycle simd instructions to do the search
/// which is more efficient, than looking inside the header first.
/// The header stores the size in the first 5 bits of the first byte of data_. The following 3 bits indicate
/// whether the pd contains a 0, the pd is overflowed and ?nothing? respectively
/// This gives a header size of one byte. Hence, everything after that is a key. Giving the ability to store 31 bytes
class pocket_dictionary : public interfaces::i_pocket_dictionary
{
public:
  pocket_dictionary()
  {
    data_ = new uint8_t[32]();
  }

  ~pocket_dictionary() override
  {
    delete[] data_;
  }

  uint8_t& operator[](const uint8_t index) const override
  {
    return data_[1 + index];
  }

  [[nodiscard]] bool query(uint8_t q, uint8_t r) const override
  {
    // q may be ignored as we don't have lists, yet the interface enforces it
    (void)q;
    if(r == 0)
    {
      return contains_zero();
    }
    // loadu may be used, as we never cross cache lines
    auto avx_reg{_mm256_loadu_si256(reinterpret_cast<__m256i*>(data_))};

    // ignore lowest byte
    _mm256_and_si256(avx_reg, util::_mm256i_bit_mask_no_header::value());
    
    auto cmp_result{_mm256_cmpeq_epi8(avx_reg, _mm256_set1_epi8(r))};
    return (_mm256_movemask_epi8(cmp_result) != 0);
  }

  void insert(uint8_t q, uint8_t r) override
  {
    if(query(q, r))
    {
      return;
    }

    data_[size()] = r;
    increase_size();
  }

  [[nodiscard]] uint8_t size() const override
  {
    return (*data_ & util::bit_mask_left<uint8_t, 4>::value);
  }



  void evict_max() override
  {
  }

  void max_move_procedure() override
  {
  }

  [[nodiscard]] uint8_t max() const override
  {
    return 0;
  }

  void mark_overflowed() override
  {
  }

  [[nodiscard]] bool overflowed() const override
  {
    return true;
  }

private:

  void increase_size()
  {
    *data_ = static_cast<uint8_t>(((*data_ & util::bit_mask_left<uint8_t, 4>::value >> 3) + 1) << 3); 
  }

  bool contains_zero() const
  {
    return util::bit_mask_position<uint8_t, 6>::value & *data_;
  }


  uint8_t* data_;
};
} // namespace prefix::adapted
#endif //INCLUDE_PREFIX_ADAPTED_POCKET_DICTIONARY_HPP
