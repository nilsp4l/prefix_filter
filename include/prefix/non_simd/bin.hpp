//
// Created by nils on 11/15/24.
//

#ifndef INCLUDE_PREFIX_NON_SIMD_BIN_H
#define INCLUDE_PREFIX_NON_SIMD_BIN_H

#include <bit>
#include <array>
#include <iostream>
#include "prefix/interfaces/i_bin.hpp"
#include "prefix/util/masks.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"

namespace prefix::non_simd
{

/// this is the naive implementation used mainly for comparison and base of the optimized ones, as
/// it is much easier to understand.
/// !Not optimized at all!
template<uint8_t k>
class bin : public interfaces::i_bin
{
public:
  bin()
  {
    static_assert(k <= 25);
  }

  ~bin() override = default;

  uint8_t& operator[](const uint8_t index) const override
  {
    return pd_[index];
  }

  [[nodiscard]] constexpr bool query(uint8_t fp) const override
  {
    return true;
  }

  constexpr void insert(uint8_t fp) override
  {

  }

  [[nodiscard]] constexpr uint8_t size() const override
  {
    return pd_.size();
  }


private:
  pocket_dictionary<k> pd_{};

};
} // namespace prefix::non_simd

#endif // INCLUDE_PREFIX_NON_SIMD_BIN_H
