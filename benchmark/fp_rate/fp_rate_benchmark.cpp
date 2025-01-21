//
// Created by nils on 1/18/25.
//

#include "fp_rate_benchmark.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "prefix/adapted/bin.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/bin.hpp"
#include "prefix/prefix_filter.hpp"
#include <iostream>

int main()
{
  constexpr std::size_t elements_to_store{5000000};
  constexpr std::size_t bins_size{static_cast<std::size_t>(elements_to_store * 4 / (0.95 * 25))};

  fp_rate_benchmark<prefix::prefix_filter<uint64_t,
                                          prefix::bin<25, prefix::simd::pocket_dictionary<25>>,
                                          bins_size>>
    simd_filter_benchmark;

  fp_rate_benchmark<prefix::prefix_filter<uint64_t,
                                          prefix::adapted::bin,
                                          bins_size>>
    adapted_filter_benchmark;
  std::cout << std::to_string(simd_filter_benchmark.random_benchmark(elements_to_store, elements_to_store * 10))
            << std::endl;
  //std::cout << std::to_string(adapted_filter_benchmark.random_benchmark(elements_to_store)) << std::endl;

}