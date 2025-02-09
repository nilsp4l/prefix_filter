//
// Created by nils on 11/15/24.
//

#ifndef PREFIX_BIN_HPP
#define PREFIX_BIN_HPP

#include <bit>
#include <array>
#include <functional>
#include "util/masks.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "util/hash_functions.hpp"

namespace prefix
{
template<typename pocket_dictionary_t>
class bin
{
public:


  static constexpr std::size_t maximum_size{pocket_dictionary_t::maximum_size};

  static constexpr bool query(uint16_t fp, uint8_t* data)
  {
    return pocket_dictionary_t::query(util::most_significant_based_fp<maximum_size>::fingerprint(fp),
      static_cast<uint8_t>(fp),
      data);
  }

  static constexpr std::optional<uint8_t> insert(uint16_t fp, uint8_t* data)
  {
    auto q{util::most_significant_based_fp<maximum_size>::fingerprint(fp)};
    auto r{static_cast<uint8_t>(fp)};


    // left that here, because it is valid to have that uncommented to ensure. that way there are no elements stored twice.
    // anyways, allowing elements stored twice will not change much about the fp-rate (it is very unlikely to happen), yet performance-wise it does.
    // disallowing it on the other hand could save memory
    /*
    if (query(fp, data))
    {
      return std::nullopt;
    }
    */


    if (pocket_dictionary_t::size(data) == maximum_size && !pocket_dictionary_t::overflowed(data))
    {
      pocket_dictionary_t::mark_overflowed(data);
      pocket_dictionary_t::max_move_procedure(data);
    }

    if (!pocket_dictionary_t::overflowed(data))
    {
      pocket_dictionary_t::insert(q,
        r,
        data);
      return std::nullopt;
    }

    uint8_t max{pocket_dictionary_t::max(data)};

    if (greater_than_max(fp, data))
    {
      return static_cast<uint8_t>(fp);
    }

    pocket_dictionary_t::evict_max(data);
    pocket_dictionary_t::insert(q, r, data);
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

  static constexpr bool greater_than_max(uint16_t fp, uint8_t* data)
  {
    uint8_t q{util::most_significant_based_fp<pocket_dictionary_t::maximum_size>::fingerprint(fp)};
    uint8_t r{static_cast<uint8_t>(fp)};
    return (r > pocket_dictionary_t::max(data) && q == pocket_dictionary_t::get_biggest_q(data))
      || (q > pocket_dictionary_t::get_biggest_q(data));
  }

  // for benchmark
  static constexpr inline std::size_t get_byte_size()
  {
    return 32;
  }

  static inline std::string to_string()
  {
    return pocket_dictionary_t::to_string();
  }

};
} // namespace prefix

#endif // PREFIX_BIN_HPP
