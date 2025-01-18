#include "prefix/simd/pocket_dictionary.hpp"
#include "bloom/bloom_filter.hpp"
#include <iostream>
#include "prefix/prefix_filter.hpp"
#include "prefix/adapted/bin.hpp"

int main()
{

  prefix::prefix_filter<uint64_t, prefix::adapted::bin, 265> pf{};

  for (uint64_t i{0}; i < 252'000'000; ++i)
  {
    pf.insert(i);
  }


  return 0;
}
