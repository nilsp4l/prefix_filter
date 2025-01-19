//
// Created by nils on 11/15/24.
//

#ifndef INCLUDE_PREFIX_BIN_HPP
#define INCLUDE_PREFIX_BIN_HPP

#include <bit>
#include <array>
#include <iostream>
#include <functional>
#include "util/masks.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "util/hash_functions.hpp"

namespace prefix
{
template<uint8_t k, typename pocket_dictionary_t>
class bin
{
public:


  static constexpr bool query(uint8_t fp, uint8_t* data)
  {
    //std::cout << std::to_string(pocket_dictionary_t::max()) << std::endl;
    return pocket_dictionary_t::query(util::most_significant_based_fp<k>::fingerprint(fp), fp, data);
  }

  static constexpr std::optional<uint8_t> insert(uint8_t fp, uint8_t* data)
  {
    if (pocket_dictionary_t::size(data) == k && !pocket_dictionary_t::overflowed(data))
    {
      pocket_dictionary_t::mark_overflowed(data);
      pocket_dictionary_t::max_move_procedure(data);
    }

    if (!pocket_dictionary_t::overflowed(data))
    {
      pocket_dictionary_t::insert(util::most_significant_based_fp<k>::fingerprint(fp), fp, data);
      return std::nullopt;
    }

    uint8_t max{pocket_dictionary_t::max(data)};

    if (fp > max)
    {
      return fp;
    }

    pocket_dictionary_t::evict_max(data);
    pocket_dictionary_t::insert(util::most_significant_based_fp<k>::fingerprint(fp), fp, data);
    pocket_dictionary_t::max_move_procedure(data);

    return max;
  }

  static constexpr uint8_t size(uint8_t* data)
  {
    return pocket_dictionary_t::size(data);
  }

  static constexpr bool overflowed(uint8_t* data)
  {
    return pocket_dictionary_t::overflowed(data);
  }

  static constexpr bool greater_than_max(uint8_t fp, uint8_t* data)
  {
    return fp > pocket_dictionary_t::max(data);
  }

};
} // namespace prefix

#endif // INCLUDE_PREFIX_BIN_HPP
