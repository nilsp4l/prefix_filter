//
// Created by nils on 1/21/25.
//

#include "prefix/spare/prefix_adapted/bin.hpp"
#include "util/masks.hpp"

#include <immintrin.h>

namespace prefix::spare::prefix_adapted
{
bool bin::query(uint8_t r, uint8_t* data)
{
  if (!size(data))
  {
    return false;
  }

// load may be used, as we assume alignment
  auto pd_reg{_mm256_load_si256(reinterpret_cast<__m256i*>(data))};

// ignore the lowest byte

  auto cmp_result{_mm256_cmpeq_epi8(pd_reg, _mm256_set1_epi8(static_cast<int8_t>(r)))};

  return ((_mm256_movemask_epi8(cmp_result) & (util::bit_mask_right_rt<int>::value(size(data))) << 1) != 0);
}

void bin::insert(uint8_t r, uint8_t* data)
{
  if (query(r, data))
  {
    return;
  }

  data[size(data) + 1] = r;

  increase_size(data);

}

uint8_t bin::size(uint8_t* data)
{
  return (*data & util::bit_mask_right<uint8_t, 5>::value);
}

void bin::increase_size(uint8_t* data)
{
  // we may ignore that the most significant bit is set, as we will never reach the point where incrementing will change that
  // as we are just manipulating the least significant 5 bits and not more here
  ++(*data);
}
}// namespace prefix::spare::prefix_adapted