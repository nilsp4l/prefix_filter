//
// Created by nils on 11/15/24.
//

#ifndef INCLUDE_PREFIX_NON_SIMD_BIN_H
#define INCLUDE_PREFIX_NON_SIMD_BIN_H

#include <bit>
#include <array>
#include <iostream>
#include <functional>
#include "prefix/interfaces/i_bin.hpp"
#include "prefix/util/masks.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"

namespace prefix::non_simd
{


template<uint8_t k>
class bin : public interfaces::i_bin
{
public:
  explicit bin(std::function<uint8_t(uint8_t)> mini_hash)
    : mini_hash_(std::move(mini_hash))
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
    return pd_.query(mini_hash_(fp), fp);
  }

  constexpr void insert(uint8_t fp) override
  {
    pd_.insert(mini_hash_(fp), fp);
  }

  [[nodiscard]] constexpr uint8_t size() const override
  {
    return pd_.size();
  }


private:
  pocket_dictionary<k> pd_{};
  std::function<uint8_t(uint8_t)> mini_hash_;

};
} // namespace prefix::non_simd

#endif // INCLUDE_PREFIX_NON_SIMD_BIN_H
