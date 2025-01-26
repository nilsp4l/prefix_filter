//
// Created by nils on 1/21/25.
//

#ifndef PREFIX_PREFIX_FILTER_FACTORY_HPP
#define PREFIX_PREFIX_FILTER_FACTORY_HPP

#include "prefix/spare/types.hpp"
#include "prefix/spare/prefix_adapted/prefix_adapted.hpp"
#include "prefix/bin_types.hpp"

namespace prefix
{

template<bin_types bin_type>
struct bin_t_choice
{
};

template<>
struct bin_t_choice<bin_types::non_simd>
{
  using type = bin<non_simd::pocket_dictionary<25>>;
};

template<>
struct bin_t_choice<bin_types::simd>
{
  using type = bin<simd::pocket_dictionary<25>>;
};

template<>
struct bin_t_choice<bin_types::adapted>
{
  using type = adapted::bin;
};

template<typename key_t, bin_types bin_type, spare::types spare_type, std::size_t elements_to_store>
struct prefix_filter_factory
{


  // need the private part above, as the constexpr values are not found otherwise
private:
  using bin_t = typename bin_t_choice<bin_type>::type;
  // to calculate the size of the spare, we need to evaluate elements_to_store / (sqrt(2 * pi * k)
  // as there is no sqrt function constexpr, we need to hardcode it
  constexpr static double sqrt_2_pi_k()
  {
    if constexpr (bin_t::maximum_size == 25)
    {
      return 12.533141373155003;
    }

    if constexpr (bin_t::maximum_size == 15)
    {
      return 9.708129562778495;
    }

  }

  constexpr static std::size_t spare_size{static_cast<std::size_t>(1.1 * elements_to_store / sqrt_2_pi_k())};

  constexpr static std::size_t prefix_adapted_no_bins
    {static_cast<std::size_t>((1.3 * spare_size)
      / (0.95 * spare::prefix_adapted::bin::maximum_size))};


public:
  constexpr static std::size_t
    no_bins{static_cast<std::size_t>((1. * elements_to_store) / (0.95 * bin_t::maximum_size))};

  constexpr static auto produce()
  {
    if constexpr (spare_type == spare::types::bloom)
    {
      constexpr auto sizes{prefix::spare::calculate_bloom_size<static_cast<std::size_t>(2 * spare_size)>()};
      return prefix_filter<key_t, bin_t, prefix::spare::bloom_filter<uint16_t, (sizes[0] >> 3), sizes[1]>, no_bins>{};
    }

    if constexpr (spare_type == spare::types::prefix_adapted)
    {
      return prefix_filter<key_t,
                           bin_t,
                           prefix::spare::prefix_adapted::filter<uint8_t,
                                                                 prefix_adapted_no_bins>,
                           no_bins>{};
    }

    // NOT REACHED

  }

  using filter_t = decltype(produce());

};

};
#endif //PREFIX_PREFIX_FILTER_FACTORY_HPP
