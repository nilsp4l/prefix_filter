//
// Created by nils on 1/18/25.
//

#include "fp_rate_benchmark.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "prefix/adapted/bin.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/bin.hpp"
#include "prefix/prefix_filter.hpp"
#include "prefix/prefix_filter_factory.hpp"
#include <iostream>

int main()
{
  constexpr std::size_t elements_to_store{5000000};
  fp_rate_benchmark<decltype(prefix::prefix_filter_factory<uint64_t,
                                                           prefix::bin<
                                                             prefix::simd::pocket_dictionary<
                                                               25>>,
                                                           prefix::spare::types::bloom,
                                                           elements_to_store>::produce())>
    simd_filter_benchmark;
  /*
  fp_rate_benchmark<decltype(prefix::prefix_filter_factory<uint64_t,
                                                           prefix::adapted::bin,
                                                           prefix::spare::types::prefix_adapted,
                                                           elements_to_store>::produce())>
    adapted_filter_benchmark;
  */

  std::cout << std::to_string(simd_filter_benchmark.random_benchmark(elements_to_store, UINT64_MAX))
            << std::endl;
  //std::cout << std::to_string(adapted_filter_benchmark.random_benchmark(elements_to_store, UINT64_MAX)) << std::endl;

}