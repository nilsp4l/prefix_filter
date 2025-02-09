//
// Created by nils on 1/18/25.
//

#include "fp_rate_benchmark.hpp"
#include "variable_size_filter_pack.hpp"
#include "prefix/prefix_filter.hpp"
#include "prefix/prefix_filter_factory.hpp"
#include <iostream>

#include <fstream>

int main()
{
  constexpr std::size_t elements_to_store{static_cast<std::size_t>(252'000'000)};
  // precalculated and hardcoded to have ~0.55 % fp rate, which helps benchmark
  constexpr double bloom_bytes_per_key{1.4069570714285715};
  constexpr std::size_t hashes{11};

  std::string fp_rates{fp_rate_benchmark::random_benchmark<prefix::prefix_filter_factory<uint64_t,
                                                                                         prefix::bin_types::simd,
                                                                                         prefix::spare::types::bloom,
                                                                                         elements_to_store>::filter_t,
                                                           prefix::prefix_filter_factory<uint64_t,
                                                                                         prefix::bin_types::adapted,
                                                                                         prefix::spare::types::bloom,
                                                                                         elements_to_store>::filter_t,
                                                           prefix::prefix_filter_factory<uint64_t,
                                                                                         prefix::bin_types::simd,
                                                                                         prefix::spare::types::prefix_adapted,
                                                                                         elements_to_store>::filter_t,
                                                           prefix::prefix_filter_factory<uint64_t,
                                                                                         prefix::bin_types::adapted,
                                                                                         prefix::spare::types::prefix_adapted,
                                                                                         elements_to_store>::filter_t,
                                                           prefix::spare::bloom::bloom_filter<uint64_t,
                                                                                              (static_cast<std::size_t>(
                                                                                                bloom_bytes_per_key
                                                                                                  * elements_to_store)),
                                                                                              hashes>>(
    elements_to_store)};

  std::fstream fp_rates_csv;
  fp_rates_csv.open("fp_rates.csv", std::ios::out | std::ios::trunc);
  fp_rates_csv << fp_rates;
  fp_rates_csv.close();
  std::cout << fp_rates << std::endl;


  std::string dynamic_size_fp_rate_simd
    {fp_rate_benchmark::random_benchmark_different_sizes(variable_size_filter_pack<elements_to_store,
                                                                                   prefix::bin_types::simd>::get_filters(),
      elements_to_store)};
  std::string dynamic_size_fp_rate_adapted
    {fp_rate_benchmark::random_benchmark_different_sizes(variable_size_filter_pack<elements_to_store,
                                                                                   prefix::bin_types::adapted>::get_filters(),
      elements_to_store)};

  std::fstream dynamic_size_fp_rates_simd;
  dynamic_size_fp_rates_simd.open("dynamic_size_fp_rates_simd.csv", std::ios::out | std::ios::trunc);
  dynamic_size_fp_rates_simd << dynamic_size_fp_rate_simd;
  dynamic_size_fp_rates_simd.close();

  std::fstream dynamic_size_fp_rates_adapted;
  dynamic_size_fp_rates_adapted.open("dynamic_size_fp_rates_adapted.csv", std::ios::out | std::ios::trunc);
  dynamic_size_fp_rates_adapted << dynamic_size_fp_rate_adapted;
  dynamic_size_fp_rates_adapted.close();

  std::cout << dynamic_size_fp_rate_simd << "\n\n" << dynamic_size_fp_rate_adapted << std::endl;

}