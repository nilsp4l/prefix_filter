#include "prefix/simd/pocket_dictionary.hpp"
#include "bloom/bloom_filter.hpp"
#include <iostream>
#include "prefix/prefix_filter.hpp"
#include "prefix/adapted/bin.hpp"
#include <random>
#include "cuckoo/cuckoo_filter.hpp"

int main()
{

  prefix::prefix_filter<uint64_t, prefix::bin<25, prefix::simd::pocket_dictionary<25>>, 20106690> pf{};

  std::random_device random_device;
  //std::array<uint64_t, 252'000'000> values_;
  auto mers{std::mt19937(random_device())};
  auto distribution{std::uniform_int_distribution<uint64_t>(0, UINT64_MAX)};
  /*
  std::generate(values_.begin(), values_.end(), [&]()
  {
    return distribution(mers);
  });
  */

  for (std::size_t i{0}; i < 252'000'000; ++i)
  {
    pf.insert(distribution(mers));
  }


  return 0;
}
