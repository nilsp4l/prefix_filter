//
// Created by nils on 1/21/25.
//

#ifndef PREFIX_PREFIX_FILTER_FACTORY_HPP
#define PREFIX_PREFIX_FILTER_FACTORY_HPP

#include "prefix/spare/types.hpp"
#include "prefix/spare/prefix_adapted/prefix_adapted.hpp"

namespace prefix
{


template<typename key_t, typename bin_t, spare::types spare_t, std::size_t elements_to_store>
struct prefix_filter_factory
{
public:
  constexpr static std::size_t
    no_bins{static_cast<std::size_t>((elements_to_store * 1.5) / (0.95 * bin_t::maximum_size))};

  constexpr static auto produce()
  {
    if constexpr (spare_t == spare::types::bloom)
    {
      return prefix_filter<key_t, bin_t, prefix::spare::bloom_filter<uint16_t, 10485706>, no_bins>{};
    }

    if constexpr (spare_t == spare::types::prefix_adapted)
    {
      return prefix_filter<key_t,
                           bin_t,
                           prefix::spare::prefix_adapted::filter<uint64_t,
                                                                 25277293>,
                           no_bins>{};
    }

    // NOT REACHED

  }

};

};
#endif //PREFIX_PREFIX_FILTER_FACTORY_HPP
