#include "prefix/simd/pocket_dictionary.hpp"
#include "prefix/spare/bloom/bloom_filter.hpp"
#include <iostream>
#include "prefix/prefix_filter.hpp"
#include "prefix/adapted/bin.hpp"
#include <random>

int main()
{

  std::cout << std::to_string(util::two_wise::fp(0x86310f8a30eb15b3) % 200) << std::endl;
  return 0;
}
