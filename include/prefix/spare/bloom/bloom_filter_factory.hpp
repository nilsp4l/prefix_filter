//
// Created by nils on 1/31/25.
//

#ifndef PREFIX_SPARE_BLOOM_FACTORY_HPP
#define PREFIX_SPARE_BLOOM_FACTORY_HPP

#include "prefix/spare/bloom/bloom_filter.hpp"


namespace prefix::spare::bloom
{

template<typename key_t, std::size_t size>
class bloom_filter_factory
{
public:
private:
  // see https://stackoverflow.com/questions/658439/how-many-hash-functions-does-my-bloom-filter-need
  constexpr static std::array<std::size_t, 2> calculate_bloom_size()
  {
    auto n{static_cast<double>(size)};

    // ln(0.001) as 0.1 % is the desired false positive rate
    double ln_p{-6.90775527898};

    // ln(2)
    double ln_2{0.69314718056};

    auto m{static_cast<std::size_t>(-n * ln_p / (ln_2 * ln_2))};
    auto k{static_cast<size_t>(static_cast<double>(m) / n * ln_2)};
    return {m, k};

  };

public:
  static constexpr inline auto produce()
  {
    constexpr auto sizes{calculate_bloom_size()};
    return bloom_filter<key_t, (sizes[0] >> 3), sizes[1]>();
  }

  using filter_t = decltype(produce());
};
};

#endif //PREFIX_SPARE_BLOOM_FACTORY_HPP
