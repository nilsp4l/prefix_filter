//
// Created by nils on 1/27/25.
//

#ifndef BENCHMARK_FP_RATE_VARIABLE_SIZE_FILTER_PACK_HPP
#define BENCHMARK_FP_RATE_VARIABLE_SIZE_FILTER_PACK_HPP


#include "prefix/prefix_filter_factory.hpp"
#include "prefix/prefix_filter.hpp"
#include "prefix/spare/bloom/bloom_filter_factory.hpp"
#include "fp_rate_benchmark.hpp"


// this whole class just exists, because google benchmark forces everything down to c++17 and double as a non-typename template parameter is only allowed for c++20 and further
// i.e. it is not possible to have the factory itself create dynamic sized filters, with a template parameter
template<std::size_t elements_to_store, prefix::bin_types bin_type>
class variable_size_filter_pack
{
public:
  using factory_t = typename prefix::prefix_filter_factory<uint64_t,
                                                           bin_type,
                                                           prefix::spare::types::bloom,
                                                           elements_to_store>;

  static constexpr inline std::size_t no_bins()
  {
    return factory_t::no_bins;
  }
  using bin_t = typename prefix::bin_t_choice<bin_type>::type;

  static constexpr inline auto get_filters()
  {

    return std::make_tuple(fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                                             bin_t,
                                                                             typename prefix::spare::bloom::bloom_filter_factory<
                                                                               std::pair<std::size_t, uint8_t>,
                                                                               2 * factory_t::spare_size>::filter_t,
                                                                             static_cast<std::size_t>(1.
                                                                               * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        typename prefix::spare::bloom::bloom_filter_factory<
                                                          std::pair<std::size_t, uint8_t>,
                                                          2 * factory_t::spare_size>::filter_t,
                                                        static_cast<std::size_t>(1.1 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        typename prefix::spare::bloom::bloom_filter_factory<
                                                          std::pair<std::size_t, uint8_t>,
                                                          2 * factory_t::spare_size>::filter_t,
                                                        static_cast<std::size_t>(1.2 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        typename prefix::spare::bloom::bloom_filter_factory<
                                                          std::pair<std::size_t, uint8_t>,
                                                          2 * factory_t::spare_size>::filter_t,
                                                        static_cast<std::size_t>(1.3 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        typename prefix::spare::bloom::bloom_filter_factory<
                                                          std::pair<std::size_t, uint8_t>,
                                                          2 * factory_t::spare_size>::filter_t,
                                                        static_cast<std::size_t>(1.4 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        typename prefix::spare::bloom::bloom_filter_factory<
                                                          std::pair<std::size_t, uint8_t>,
                                                          2 * factory_t::spare_size>::filter_t,
                                                        static_cast<std::size_t>(1.5 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        typename prefix::spare::bloom::bloom_filter_factory<
                                                          std::pair<std::size_t, uint8_t>,
                                                          2 * factory_t::spare_size>::filter_t,
                                                        static_cast<std::size_t>(1.6 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        typename prefix::spare::bloom::bloom_filter_factory<
                                                          std::pair<std::size_t, uint8_t>,
                                                          2 * factory_t::spare_size>::filter_t,
                                                        static_cast<std::size_t>(2. * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        typename prefix::spare::bloom::bloom_filter_factory<
                                                          std::pair<std::size_t, uint8_t>,
                                                          2 * factory_t::spare_size>::filter_t,
                                                        static_cast<std::size_t>(2.5 * no_bins())>>{});

  }
};

#endif //BENCHMARK_FP_RATE_VARIABLE_SIZE_FILTER_PACK_HPP
