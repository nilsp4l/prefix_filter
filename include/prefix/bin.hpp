//
// Created by nils on 11/15/24.
//

#ifndef INCLUDE_PREFIX_BIN_HPP
#define INCLUDE_PREFIX_BIN_HPP

#include <bit>
#include <array>
#include <iostream>
#include <functional>
#include "prefix/interfaces/i_bin.hpp"
#include "util/masks.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "util/hash_functions.hpp"

namespace prefix
{
template<uint8_t k, typename pocket_dictionary_t>
class bin : public interfaces::i_bin
{
public:
  explicit bin()
  {
    static_assert(k <= 25);
  }

  ~bin() override = default;

  bin(bin<k, pocket_dictionary_t>&& other) noexcept = default;
  bin& operator=(bin<k, pocket_dictionary_t>&& other) noexcept = default;

  uint8_t& operator[](const uint8_t index) const override
  {
    return pd_[index];
  }

  [[nodiscard]] bool query(uint8_t fp) const override
  {
    return pd_.query(util::most_significant_based_fp<k>::fingerprint(fp), fp);
  }

  std::optional<uint8_t> insert(uint8_t fp) override
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

  [[nodiscard]] uint8_t size() const override
  {
    return pd_.size();
  }

  [[nodiscard]] bool overflowed() const override
  {
    return pd_.overflowed();
  }

  [[nodiscard]] bool smaller_than_max(uint8_t fp) override
  {
    return fp < pd_.max();
  }

private:
  pocket_dictionary_t pd_{};

};
} // namespace prefix

#endif // INCLUDE_PREFIX_BIN_HPP
