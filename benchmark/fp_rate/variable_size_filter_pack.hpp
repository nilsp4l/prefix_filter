//
// Created by nils on 1/27/25.
//

#ifndef BENCHMARK_FP_RATE_VARIABLE_SIZE_FILTER_PACK_HPP
#define BENCHMARK_FP_RATE_VARIABLE_SIZE_FILTER_PACK_HPP


#include "prefix/prefix_filter_factory.hpp"
#include "prefix/prefix_filter.hpp"
#include "prefix/spare/bloom/bloom_filter.hpp"
#include "fp_rate_benchmark.hpp"

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
    constexpr auto sizes{prefix::spare::calculate_bloom_size<static_cast<std::size_t>(2 * factory_t::spare_size)>()};


    return std::make_tuple(fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                                             bin_t,
                                                                             prefix::spare::bloom_filter<std::pair<std::size_t,
                                                                                                                   uint8_t>,
                                                                                                         (sizes[0]
                                                                                                           >> 3),
                                                                                                         sizes[1]>,
                                                                             static_cast<std::size_t>(1.
                                                                               * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        prefix::spare::bloom_filter<std::pair<std::size_t, uint8_t>,
                                                                                    (sizes[0] >> 3),
                                                                                    sizes[1]>,
                                                        static_cast<std::size_t>(1.1 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        prefix::spare::bloom_filter<std::pair<std::size_t, uint8_t>,
                                                                                    (sizes[0] >> 3),
                                                                                    sizes[1]>,
                                                        static_cast<std::size_t>(1.2 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        prefix::spare::bloom_filter<std::pair<std::size_t, uint8_t>,
                                                                                    (sizes[0] >> 3),
                                                                                    sizes[1]>,
                                                        static_cast<std::size_t>(1.3 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        prefix::spare::bloom_filter<std::pair<std::size_t, uint8_t>,
                                                                                    (sizes[0] >> 3),
                                                                                    sizes[1]>,
                                                        static_cast<std::size_t>(1.4 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        prefix::spare::bloom_filter<std::pair<std::size_t, uint8_t>,
                                                                                    (sizes[0] >> 3),
                                                                                    sizes[1]>,
                                                        static_cast<std::size_t>(1.5 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        prefix::spare::bloom_filter<std::pair<std::size_t, uint8_t>,
                                                                                    (sizes[0] >> 3),
                                                                                    sizes[1]>,
                                                        static_cast<std::size_t>(1.6 * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        prefix::spare::bloom_filter<std::pair<std::size_t, uint8_t>,
                                                                                    (sizes[0] >> 3),
                                                                                    sizes[1]>,
                                                        static_cast<std::size_t>(2. * no_bins())>>{},
      fp_rate_benchmark_iteration<prefix::prefix_filter<uint64_t,
                                                        bin_t,
                                                        prefix::spare::bloom_filter<std::pair<std::size_t, uint8_t>,
                                                                                    (sizes[0] >> 3),
                                                                                    sizes[1]>,
                                                        static_cast<std::size_t>(2.5 * no_bins())>>{});

  }
};

#endif //BENCHMARK_FP_RATE_VARIABLE_SIZE_FILTER_PACK_HPP
