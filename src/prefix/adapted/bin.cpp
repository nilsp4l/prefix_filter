//
// Created by nils on 1/5/25.
//

#include "prefix/adapted/bin.hpp"
namespace prefix::adapted
{


bool bin::query(uint16_t r, uint8_t* data)
{
  auto* data_16{reinterpret_cast<uint16_t*>(data)};

  if (!size(data))
  {
    return false;
  }

// load may be used, as we assume alignment
  auto pd_reg{_mm256_load_si256(reinterpret_cast<__m256i*>(data_16))};

// ignore the lowest byte

  auto cmp_result{_mm256_cmpeq_epi16(pd_reg, _mm256_set1_epi16(static_cast<int16_t>(r)))};

  return ((_mm256_movemask_epi8(cmp_result)
    & (util::bit_mask_right_rt<int>::value(static_cast<uint8_t>(size(data) << 1)) << 2)) != 0);
}

std::optional<uint8_t> bin::insert(uint16_t r, uint8_t* data)
{

  auto* data_16{reinterpret_cast<uint16_t*>(data)};
// Because of the overflowed procedure:
// query code is copied here and not called, because the _mm256_set1_epi8 instruction is sequential
// doing that twice would be wasteful. Furthermore, the loaded register is still relevant

  auto pd_reg{_mm256_load_si256(reinterpret_cast<__m256i*>(data_16))};

  auto r_reg{_mm256_set1_epi16(static_cast<int16_t>(r))};

  auto cmp_result{_mm256_cmpeq_epi16(pd_reg, r_reg)};

  auto valid_elements_mask{util::bit_mask_right_rt<int>::value(static_cast<uint8_t>(size(data) << 1)) << 2};

  if ((_mm256_movemask_epi8(cmp_result) & valid_elements_mask) != 0)
  {
    return std::nullopt;
  }

  if (size(data) == maximum_size)
  {
    set_overflowed(data);
  }

  if (!overflowed(data))
  {
    data_16[size(data) + 1] = r;

    increase_size(data);
    return std::nullopt;

  }

// static cast to uint is necessary to not shift arithmetically
  auto
    r_is_max{static_cast<uint32_t>(_mm256_movemask_epi8(_mm256_cmpeq_epi16(r_reg, _mm256_max_epu16(r_reg, pd_reg))))};

// overflowed procedure
  auto max_element_positions{(~r_is_max) & static_cast<uint32_t>(valid_elements_mask)};

  if (max_element_positions == 0)
  {
    return static_cast<uint8_t>(r);

  }

  // shift out the header
  max_element_positions >>= 2;

  uint8_t max_index{0};

  uint16_t max_value{0};

  uint8_t current_index{0};

  while (max_element_positions)
  {
    if ((max_element_positions & util::bit_mask_right<int, 2>::value)
      && data_16[1 + current_index] > max_value)
    {
      max_value = data_16[1 + current_index];
      max_index = current_index;
    }
    max_element_positions >>= 2;
    ++current_index;
  }

  data_16[1 + max_index] = r;

  return static_cast<uint8_t>(max_value);

}

uint8_t bin::size(uint8_t* data)
{
  auto* data_16{reinterpret_cast<uint16_t*>(data)};
  return (*data_16 & util::bit_mask_right<uint8_t, 4>::value);
}

bool bin::overflowed(uint8_t* data)
{
  auto* data_16{reinterpret_cast<uint16_t*>(data)};
  return util::bit_mask_position<uint8_t, 0>::value & *data_16;
}

void bin::set_overflowed(uint8_t* data)
{
  auto* data_16{reinterpret_cast<uint16_t*>(data)};
  (*data_16) |= 0x80;
}

bool bin::greater_than_max(uint16_t fp, uint8_t* data)
{
  auto* data_16{reinterpret_cast<uint16_t*>(data)};
  auto r_reg{_mm256_set1_epi16(static_cast<int16_t>(fp))};
  auto pd_reg{_mm256_loadu_si256(reinterpret_cast<__m256i*>(data_16))};

  if (_mm256_movemask_epi8(_mm256_cmpeq_epi16(r_reg, pd_reg)))
  {
    return false;
  }

  return _mm256_movemask_epi8(_mm256_cmpeq_epi16(r_reg, _mm256_max_epu16(r_reg, pd_reg)));
}

void bin::increase_size(uint8_t* data)
{
  auto* data_16{reinterpret_cast<uint16_t*>(data)};
  // we may ignore that the most significant bit is set, as we will never reach the point where incrementing will change that
  // as we are just manipulating the least significant 5 bits and not more here
  ++(*data_16);
}
}