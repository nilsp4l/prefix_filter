//
// Created by nils on 12/12/24.
//

#ifndef PREFIX_SIMD_POCKET_DICTIONARY_HPP
#define PREFIX_SIMD_POCKET_DICTIONARY_HPP

#include <cstdint>
#include <x86intrin.h>

#include "prefix/interfaces/i_pocket_dictionary.hpp"

namespace prefix::simd
{
template<std::size_t k>
class pocket_dictionary : public interfaces::i_pocket_dictionary
{
public:
  pocket_dictionary()
  {
    static_assert(k <= 25);
    data_ = new uint8_t[32];
  }

  ~pocket_dictionary() override
  {
    delete[] data_;
  }

  uint8_t& operator[](uint8_t index) const override
  {
    return data_[7 + index];
  }

  [[nodiscard]] constexpr bool query(uint8_t q, uint8_t r) const override
  {
    return true;
  }

  constexpr void insert(uint8_t q, uint8_t r) override
  {
    auto reg{_mm256_loadu_epi8(data_)};
  }

  [[nodiscard]] constexpr uint8_t size() const override
  {
    return 0;
  }

  constexpr void evict_max() override
  {

  }

  constexpr void max_move_procedure() override
  {

  }

  [[nodiscard]] constexpr uint8_t max() const override
  {
    return 0;
  }

  constexpr void mark_overflowed() override
  {

  }

  [[nodiscard]] constexpr bool overflowed() const override
  {
    return true;
  }


private:
  uint8_t* data_;

};

} // namespace prefix::simd


#endif //PREFIX_SIMD_POCKET_DICTIONARY_HPP
