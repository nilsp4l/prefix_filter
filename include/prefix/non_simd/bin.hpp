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
#include "util/masks.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "util/hash_functions.hpp"

namespace prefix::non_simd
{


template<uint8_t k>
class bin : public interfaces::i_bin
{
public:
  explicit bin()
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
    return pd_.query(util::most_significant_based_fp<k>::fingerprint(fp), fp);
  }

  constexpr std::optional<uint8_t> insert(uint8_t fp) override
  {
    if (pd_.size() == k && !pd_.overflowed())
    {
      pd_.mark_overflowed();
      pd_.max_move_procedure();
    }


    if (!pd_.overflowed())
    {
      pd_.insert(util::most_significant_based_fp<k>::fingerprint(fp), fp);
      return std::nullopt;
    }

    uint8_t max{pd_.max()};

    if (fp > max)
    {
      return fp;
    }

    pd_.evict_max();
    pd_.insert(util::most_significant_based_fp<k>::fingerprint(fp), fp);
    pd_.max_move_procedure();
    
    return max;
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
