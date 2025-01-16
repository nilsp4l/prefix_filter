//
// Created by nils on 1/16/25.
//

#include <benchmark/benchmark.h>
#include "prefix/prefix_filter.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "prefix/bin.hpp"


class prefix_benchmark_fixture : public benchmark::Fixture
{
public:
  void SetUp(benchmark::State& state) override
  {
    filter_ = prefix::prefix_filter<uint64_t, prefix::bin<25, prefix::simd::pocket_dictionary<25>>, 1000000>();
  }

  void TearDown(benchmark::State& state) override
  {
  }

  prefix::prefix_filter<uint64_t, prefix::bin<25, prefix::simd::pocket_dictionary<25>>, 1000000> filter_;

};

BENCHMARK_F(prefix_benchmark_fixture, insert_linear)(benchmark::State& st)
{
  for (auto _ : st)
  {
    for (uint64_t i{0}; i < 25200000; ++i)
    {
      filter_.insert(i);
    }
  }

}
BENCHMARK_MAIN();