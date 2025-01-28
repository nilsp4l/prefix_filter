//
// Created by nils on 1/18/25.
//

#ifndef BENCHMARK_FP_RATE_BENCHMARK_HPP
#define BENCHMARK_FP_RATE_BENCHMARK_HPP

#include <cinttypes>
#include <random>
#include <set>
#include <iostream>
#include <tuple>


template<typename filter_t>
class fp_rate_benchmark_iteration
{
public:
  fp_rate_benchmark_iteration() = default;

  static inline std::string filter_name()
  {
    return filter_t::to_string();
  }

  static inline std::size_t filter_byte_size()
  {
    return filter_t::get_byte_size();
  }

  static inline std::size_t filter_spare_byte_size()
  {
    return filter_t::get_spare_byte_size();
  }

  double random_benchmark(std::size_t no_elements)
  {
    std::size_t no_false_positive{0};
    std::size_t no_true_negative{0};

    std::random_device random_device;

    std::mt19937 mers{random_device()};
    auto distribution{std::uniform_int_distribution<uint64_t>(0, UINT64_MAX)};
    std::set<uint64_t> compare_set;

    for (std::size_t i{0}; i < no_elements; ++i)
    {
      auto current_random_element{distribution(mers)};
      //compare_set.insert(current_random_element);
      filter_.insert(current_random_element);
    }


    for (std::size_t i{0}; i < no_elements; ++i)
    {
      auto current_element{distribution(mers)};
      if (filter_.query(current_element))
      {
        ++no_false_positive;
      }
      else
      {
        ++no_true_negative;
      }

      /*
      if (compare_set.find(current_element) == compare_set.end() && filter_.query(current_element))
      {
        ++no_false_positive;
      }
      else if (compare_set.find(current_element) == compare_set.end() && !filter_.query(current_element))
      {
        ++no_true_negative;
      }

      if (compare_set.find(current_element) != compare_set.end() && !filter_.query(current_element))
      {
        std::cout << "FALSE NEGATIVE!" << std::endl;
      }
       */

    }

    return static_cast<double>(no_false_positive) / static_cast<double>(no_false_positive + no_true_negative);

  }


private:
  filter_t filter_;
};

template<typename... filter_t_pack>
struct fp_rate_benchmark
{
public:


  template<typename... filters>
  static std::string random_benchmark_different_sizes(std::tuple<filters...>&& benchmarks, std::size_t no_elements)
  {

    std::string to_return;
    to_return += "name,"
                 "fp_rate,"
                 "size"
                 "\n";
    std::vector<double> fp_rates{};
    std::vector<std::string> names{};
    std::vector<std::size_t> byte_sizes{};

    std::apply([&fp_rates, &byte_sizes, &names, no_elements](auto&& ... args)
    {

      (names.push_back(std::remove_reference<decltype(args)>::type::filter_name()), ...);
      (byte_sizes.push_back(std::remove_reference<decltype(args)>::type::filter_byte_size()), ...);
      ((fp_rates.push_back(args.random_benchmark(no_elements))), ...);

    }, benchmarks);;
    for (std::size_t i{0}; i < sizeof...(filters); ++i)
    {
      to_return += names[i] + "," + std::to_string(fp_rates[i]) + "," + std::to_string(byte_sizes[i]);
      if (i != sizeof...(filters) - 1)
      {
        to_return += "\n";
      }
    }

    return to_return;

  }

  static std::string random_benchmark(std::size_t no_elements)
  {
    std::string to_return;
    std::tuple<fp_rate_benchmark_iteration<filter_t_pack> ...> benchmarks{};
    constexpr std::size_t parameter_size{sizeof...(filter_t_pack)};

    to_return += "name,"
                 "fp_rate,"
                 "size_in_bytes,"
                 "spare_size_in_bytes,"
                 "inserted_elements"
                 "\n";

    std::vector<std::string> names{};
    std::vector<double> fp_rates{};
    std::vector<std::size_t> byte_sizes{};
    std::vector<std::size_t> spare_byte_sizes{};

    std::apply([&fp_rates, &names, &byte_sizes, & spare_byte_sizes, no_elements](auto&& ... args)
    {
      (names.push_back(std::remove_reference<decltype(args)>::type::filter_name()), ...);
      (byte_sizes.push_back(std::remove_reference<decltype(args)>::type::filter_byte_size()), ...);
      (spare_byte_sizes.push_back(std::remove_reference<decltype(args)>::type::filter_spare_byte_size()), ...);
      ((fp_rates.push_back(args.random_benchmark(no_elements))), ...);

    }, benchmarks);

    for (std::size_t i{0}; i < parameter_size; ++i)
    {
      to_return += names[i] + ",";
      to_return += std::to_string(fp_rates[i]) + ",";
      to_return += std::to_string(byte_sizes[i]) + ",";
      to_return += std::to_string(spare_byte_sizes[i]) + ",";
      to_return += std::to_string(no_elements);
      if (i != parameter_size - 1)
      {
        to_return += "\n";
      }
    }


    return to_return;
  }

};


#endif //BENCHMARK_FP_RATE_BENCHMARK_HPP
