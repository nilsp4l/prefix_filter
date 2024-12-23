#include "prefix/non_simd/prefix_filter.hpp"
#include "util/masks.hpp"
#include "prefix/adapted/pocket_dictionary.hpp"
#include <immintrin.h>
#include <cstdint>
#include <iostream>

int main()
{
  auto foo{util::_m256i_bit_mask_left<3>::value()};

  for (uint8_t i{0}; i < 32; ++i)
  {
    std::cout << std::to_string(reinterpret_cast<uint8_t*>(&foo)[i]) << std::endl;
  }

  return 0;
}
