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
  // need the private part above, as the constexpr values are not found otherwise
private:

  // to calculate the size of the spare, we need to evaluate elements_to_store / (sqrt(2 * pi * k)
  // as there is no sqrt function constexpr, we need to hardcode it
  constexpr static double sqrt_2_pi_k()
  {
    if constexpr (bin_t::maximum_size == 25)
    {
      return 12.533141373155003;
    }

    if constexpr (bin_t::maximum_size == 31)
    {
      return 13.956315578352589;
    }
  }

  constexpr static std::size_t prefix_adapted_no_bins
    {static_cast<std::size_t>((1.1 * elements_to_store / sqrt_2_pi_k() * 1.5)
      / (0.95 * spare::prefix_adapted::bin::maximum_size))};

public:
  constexpr static std::size_t
    no_bins{static_cast<std::size_t>((elements_to_store) / (0.95 * bin_t::maximum_size))};

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
                           prefix::spare::prefix_adapted::filter<uint8_t,
                                                                 prefix_adapted_no_bins>,
                           no_bins>{};
    }

    // NOT REACHED

  }

};

};
#endif //PREFIX_PREFIX_FILTER_FACTORY_HPP
