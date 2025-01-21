//
// Created by nils on 1/18/25.
//

#ifndef BENCHMARK_FP_RATE_BENCHMARK_HPP
#define BENCHMARK_FP_RATE_BENCHMARK_HPP

#include <cinttypes>
#include <random>
#include <set>

template<typename filter_t>
class fp_rate_benchmark
{
public:
  fp_rate_benchmark() = default;

  double random_benchmark(std::size_t no_elements, uint64_t maximum)
  {
    std::size_t no_false_positive{0};
    std::size_t no_true_negative{0};

    std::random_device random_device;

    std::mt19937 mers{random_device()};
    auto distribution{std::uniform_int_distribution<uint64_t>(0, maximum)};
    std::set<uint64_t> compare_set;

    for (std::size_t i{0}; i < no_elements; ++i)
    {
      auto current_random_element{distribution(mers)};
      compare_set.insert(current_random_element);
      filter_.insert(current_random_element);
    }

    for (std::size_t i{0}; i < maximum; ++i)
    {
      if (compare_set.find(i) == compare_set.end() && filter_.query(i))
      {
        ++no_false_positive;
      }
      else if (compare_set.find(i) == compare_set.end() && !filter_.query(i))
      {
        ++no_true_negative;
      }

    }

    return static_cast<double>(no_false_positive) / static_cast<double>(no_false_positive + no_true_negative);

  }


private:
  filter_t filter_;
};

#endif //BENCHMARK_FP_RATE_BENCHMARK_HPP
