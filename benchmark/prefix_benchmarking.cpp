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
#include "prefix/bin_types.hpp"
#include <string>


static constexpr std::size_t elements_count{252'000'000};

constexpr std::size_t bpk{11};

constexpr std::size_t hashes{11};

static constexpr std::array<std::size_t, 2> bloom_sizes{{2'836'425'457, hashes}};

static constexpr auto spare_impl{prefix::spare::types::prefix_adapted};



// workaround as auto does not work for non-static data members
template<prefix::bin_types bin_type, std::size_t elements_to_store>
using prefix_type = typename prefix::prefix_filter_factory<uint64_t,
                                                           bin_type,
                                                           spare_impl,
                                                           elements_to_store>::filter_t;

template<prefix::bin_types bin_type, std::size_t elements_to_store>
class prefix_insert_benchmark_fixture : public benchmark::Fixture
{
public:
  void SetUp(benchmark::State& state) override
  {
  }

  void TearDown(benchmark::State& state) override
  {
  }

  prefix_type<bin_type, elements_to_store>
    filter_{prefix::prefix_filter_factory<uint64_t,
                                          bin_type,
                                          spare_impl,
                                          elements_to_store>::produce()};

};

template<prefix::bin_types bin_type, std::size_t elements_to_store>
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

  prefix_type<bin_type, elements_to_store>
    filter_{prefix::prefix_filter_factory<uint64_t,
                                          bin_type,
                                          spare_impl,
                                          elements_to_store>::produce()};
  std::mt19937 mers;
  std::uniform_int_distribution<uint64_t> distribution;

};

template<prefix::bin_types bin_type, std::size_t elements_to_store>
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

  prefix_type<bin_type, elements_to_store>
    filter_{prefix::prefix_filter_factory<uint64_t,
                                          bin_type,
                                          spare_impl,
                                          elements_to_store>::produce()};
  std::array<uint64_t, elements_to_store> keys_{};

};

class bloom_query_benchmark_fixture : public benchmark::Fixture
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

  prefix::spare::bloom_filter<uint64_t, (bloom_sizes[0] >> 3), bloom_sizes[1]> filter_{};
  std::array<uint64_t, elements_count> keys_{};

};

template<prefix::bin_types bin_type, std::size_t elements_to_store>
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

  prefix_type<bin_type, elements_to_store>
    filter_{prefix::prefix_filter_factory<uint64_t,
                                          bin_type,
                                          spare_impl,
                                          elements_to_store>::produce()};
  std::array<uint64_t, 252'000'000> values_{};

};

class bloom_random_insert_from_array_benchmark_fixture : public benchmark::Fixture
{
public:
  using filter_t = prefix::spare::bloom_filter<uint64_t, (bloom_sizes[0]
    >> 3), bloom_sizes[1]>;

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

  filter_t filter_{};
  std::array<uint64_t, 252'000'000> values_{};

};

BENCHMARK_F(bloom_query_benchmark_fixture,
  query_random_bloom)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (auto key : keys_)
    {
      filter_.query(key);
    }
  }
}

BENCHMARK_TEMPLATE_F(prefix_insert_benchmark_fixture,
  insert_linear,
  prefix::bin_types::simd,
  elements_count)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{252000000}; i > 0; --i)
    {
      filter_.insert(i);
    }
  }

}

BENCHMARK_TEMPLATE_F(prefix_insert_benchmark_fixture,
  insert_linear_adapted,
  prefix::bin_types::adapted,
  elements_count)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{252000000}; i > 0; --i)
    {
      filter_.insert(i);
    }
  }

}

BENCHMARK_TEMPLATE_F(prefix_random_insert_from_array_benchmark_fixture,
  random_insert_from_array,
  prefix::bin_types::simd,
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

BENCHMARK_TEMPLATE_F(prefix_random_insert_from_array_benchmark_fixture,
  random_insert_from_array_adapted,
  prefix::bin_types::adapted,
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

BENCHMARK_F(bloom_random_insert_from_array_benchmark_fixture,
  random_insert_from_array_bloom)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (auto value : values_)
    {
      filter_.insert(value);
    }
  }
}

BENCHMARK_TEMPLATE_F(prefix_query_benchmark_fixture,
  query_random_adapted,
  prefix::bin_types::adapted,
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
  query_random,
  prefix::bin_types::simd,
  elements_count)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (auto key : keys_)
    {
      filter_.query(key);
    }
  }
}

BENCHMARK_MAIN();