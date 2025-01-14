#include "prefix/simd/pocket_dictionary.hpp"
#include "bloom/bloom_filter.hpp"
#include <iostream>
#include "prefix/prefix_filter.hpp"

int main()
{

  prefix::prefix_filter<uint64_t, 1000000> pf{};

  for (uint64_t i{0}; i < 252329329; ++i)
  {
    pf.insert(i);
  }


  return 0;
}
