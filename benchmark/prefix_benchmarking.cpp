//
// Created by nils on 1/16/25.
//

#include <benchmark/benchmark.h>
#include "prefix/prefix_filter.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/adapted/bin.hpp"
#include "prefix/bin.hpp"
#include "bloom/bloom_filter.hpp"
#include <random>


static constexpr std::size_t linear_bins_size{265};

template<typename bin_t, std::size_t no_bins>
class prefix_insert_benchmark_fixture : public benchmark::Fixture
{
public:
  void SetUp(benchmark::State& state) override
  {
  }

  void TearDown(benchmark::State& state) override
  {
  }

  prefix::prefix_filter<uint64_t, bin_t, no_bins> filter_;

};

template<typename bin_t, std::size_t no_bins>
class prefix_random_insert_benchmark_fixture : public benchmark::Fixture
{
public:
  void SetUp(benchmark::State& state) override
  {
    std::random_device random_device;

    mers = std::mt19937(random_device());
    distribution = std::uniform_int_distribution<uint64_t>(0, UINT64_MAX);
  }

  void TearDown(benchmark::State& state) override
  {
  }

  prefix::prefix_filter<uint64_t, bin_t, no_bins> filter_;
  std::mt19937 mers;
  std::uniform_int_distribution<uint64_t> distribution;

};

template<typename bin_t, std::size_t no_bins, std::size_t elements_size>
class prefix_query_benchmark_fixture : public benchmark::Fixture
{
public:
  void SetUp(benchmark::State& state) override
  {
    std::random_device random_device;

    std::mt19937 mers{random_device()};
    std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);

    std::generate(keys_.begin(), keys_.end(), [&]()
    {
      return distribution(mers);
    });

    for (uint64_t key : keys_)
    {
      filter_.insert(key);
    }
  }

  void TearDown(benchmark::State& state) override
  {
  }

  prefix::prefix_filter<uint64_t, bin_t, no_bins> filter_;
  std::array<uint64_t, elements_size> keys_{};

};

BENCHMARK_TEMPLATE_F(prefix_insert_benchmark_fixture,
  insert_linear,
  prefix::bin<25, prefix::simd::pocket_dictionary<25>>,
  linear_bins_size)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{25200000}; i > 0; --i)
    {
      filter_.insert(i);
    }
  }

}

BENCHMARK_TEMPLATE_F(prefix_insert_benchmark_fixture,
  insert_linear_adapted,
  prefix::adapted::bin,
  linear_bins_size)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{25200000}; i > 0; --i)
    {
      filter_.insert(i);
    }
  }

}

BENCHMARK_TEMPLATE_F(prefix_query_benchmark_fixture,
  query_random,
  prefix::bin<25, prefix::simd::pocket_dictionary<25>>,
  linear_bins_size,
  252'000'000)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (auto key : keys_)
    {
      filter_.query(key);
    }
  }
}

BENCHMARK_TEMPLATE_F(prefix_query_benchmark_fixture,
  query_random_adapted,
  prefix::adapted::bin,
  linear_bins_size,
  252'000'000)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (auto key : keys_)
    {
      filter_.query(key);
    }
  }
}

BENCHMARK_TEMPLATE_F(prefix_random_insert_benchmark_fixture,
  insert_random,
  prefix::bin<25, prefix::simd::pocket_dictionary<25>>,
  linear_bins_size)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{0}; i < 252'000'000; ++i)
    {
      filter_.insert(distribution(mers));
    }
  }

}

BENCHMARK_TEMPLATE_F(prefix_random_insert_benchmark_fixture,
  insert_random_adapted,
  prefix::adapted::bin,
  linear_bins_size)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{0}; i < 252'000'000; ++i)
    {
      filter_.insert(distribution(mers));
    }
  }

}

BENCHMARK_MAIN();