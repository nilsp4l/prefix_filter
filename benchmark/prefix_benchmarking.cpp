//
// Created by nils on 1/16/25.
//

#include <benchmark/benchmark.h>
#include "prefix/prefix_filter.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/adapted/bin.hpp"
#include "prefix/bin.hpp"
#include "prefix/spare/bloom/bloom_filter.hpp"
#include <random>
#include "prefix/prefix_filter_factory.hpp"
#include "prefix/spare/types.hpp"
#include "prefix/spare/prefix_adapted/prefix_adapted.hpp"


static constexpr std::size_t elements_count{252'000'000};

// workaround as auto does not work for non-static data members
template<typename bin_t, std::size_t elements_to_store>
using prefix_filter_with_bloom = decltype(prefix::prefix_filter_factory<uint64_t,
                                                                        bin_t,
                                                                        prefix::spare::types::prefix_adapted,
                                                                        elements_to_store>::produce());

template<typename bin_t, std::size_t elements_to_store>
class prefix_insert_benchmark_fixture : public benchmark::Fixture
{
public:
  void SetUp(benchmark::State& state) override
  {
  }

  void TearDown(benchmark::State& state) override
  {
  }

  prefix_filter_with_bloom<bin_t, elements_to_store>
    filter_{prefix::prefix_filter_factory<uint64_t,
                                          bin_t,
                                          prefix::spare::types::prefix_adapted,
                                          elements_to_store>::produce()};

};

template<typename bin_t, std::size_t elements_to_store>
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

  prefix_filter_with_bloom<bin_t, elements_to_store>
    filter_{prefix::prefix_filter_factory<uint64_t,
                                          bin_t,
                                          prefix::spare::types::prefix_adapted,
                                          elements_to_store>::produce()};
  std::mt19937 mers;
  std::uniform_int_distribution<uint64_t> distribution;

};

template<typename bin_t, std::size_t elements_to_store>
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

  prefix_filter_with_bloom<bin_t, elements_to_store>
    filter_{prefix::prefix_filter_factory<uint64_t,
                                          bin_t,
                                          prefix::spare::types::prefix_adapted,
                                          elements_to_store>::produce()};
  std::array<uint64_t, elements_to_store> keys_{};

};

template<typename bin_t, std::size_t elements_to_store>
class prefix_random_insert_from_array_benchmark_fixture : public benchmark::Fixture
{
public:
  void SetUp(benchmark::State& state) override
  {
    std::random_device random_device;

    auto mers{std::mt19937(random_device())};
    auto distribution{std::uniform_int_distribution<uint64_t>(0, UINT64_MAX)};

    std::generate(values_.begin(), values_.end(), [&]()
    {
      return distribution(mers);
    });
  }

  void TearDown(benchmark::State& state) override
  {
  }

  prefix_filter_with_bloom<bin_t, elements_to_store>
    filter_{prefix::prefix_filter_factory<uint64_t,
                                          bin_t,
                                          prefix::spare::types::prefix_adapted,
                                          elements_to_store>::produce()};
  std::array<uint64_t, 252'000'000> values_{};

};

class set_random_insert_fixture : public benchmark::Fixture
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

  std::set<uint64_t> set_;
  std::mt19937 mers;
  std::uniform_int_distribution<uint64_t> distribution;

};

BENCHMARK_TEMPLATE_F(prefix_insert_benchmark_fixture,
  insert_linear,
  prefix::bin<prefix::simd::pocket_dictionary<25>>,
  elements_count)(benchmark::State& st)
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
  elements_count)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{25200000}; i > 0; --i)
    {
      filter_.insert(i);
    }
  }

}

BENCHMARK_TEMPLATE_F(prefix_random_insert_from_array_benchmark_fixture,
  random_insert_from_array,
  prefix::bin<prefix::simd::pocket_dictionary<25>>,
  elements_count)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (auto value : values_)
    {
      filter_.insert(value);
    }
  }
}

/*
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

*/
BENCHMARK_TEMPLATE_F(prefix_query_benchmark_fixture,
  query_random_adapted,
  prefix::adapted::bin,
  252'000'000)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (auto key : keys_)
    {
      filter_.query(key);
    }
  }
  return;
}

BENCHMARK_TEMPLATE_F(prefix_random_insert_benchmark_fixture,
  insert_random,
  prefix::bin<prefix::simd::pocket_dictionary<25>>,
  elements_count)(benchmark::State& st)
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
  elements_count)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{0}; i < 252'000'000; ++i)
    {
      filter_.insert(distribution(mers));
    }
  }

}

BENCHMARK_DEFINE_F(set_random_insert_fixture, set_random_insert)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{0}; i < 25'200'000; ++i)
    {
      set_.insert(distribution(mers));
    }
  }
}

BENCHMARK_MAIN();