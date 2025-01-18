//
// Created by nils on 1/5/25.
//

#include "prefix/adapted/bin.hpp"
namespace prefix::adapted
{
bin::bin()
{
  data_ = new uint8_t[32]();
}

bin::~bin()
{
  delete[] data_;
}

uint8_t& bin::operator[](const uint8_t index) const
{
  return data_[1 + index];
}

[[nodiscard]] bool bin::query(uint8_t r) const
{
  if (!size())
  {
    return false;
  }

// loadu may be used, as we never cross cache lines
  auto pd_reg{_mm256_loadu_si256(reinterpret_cast<__m256i*>(data_))};

// ignore the lowest byte

  auto cmp_result{_mm256_cmpeq_epi8(pd_reg, _mm256_set1_epi8(static_cast<int8_t>(r)))};

  return ((_mm256_movemask_epi8(cmp_result) & (util::bit_mask_right_rt<int>::value(size())) << 1) != 0);
}

std::optional<uint8_t> bin::insert(uint8_t r)
{
// Because of the overflowed procedure:
// query code is copied here and not called, because the _mm256_set1_epi8 instruction is sequential
// doing that twice would be wasteful. Furthermore, the loaded register is still relevant

  auto pd_reg{_mm256_loadu_si256(reinterpret_cast<__m256i*>(data_))};

  auto r_reg{_mm256_set1_epi8(static_cast<int8_t>(r))};

  auto cmp_result{_mm256_cmpeq_epi8(pd_reg, r_reg)};

  auto valid_elements_mask{util::bit_mask_right_rt<int>::value(size()) << 1};

  if ((_mm256_movemask_epi8(cmp_result) & valid_elements_mask) != 0)
  {
    return std::nullopt;
  }

  if (size() == 31)
  {
    set_overflowed();
  }

  if (!overflowed())
  {
    data_[size() + 1] = r;

    increase_size();
    return std::nullopt;

  }

// static cast to uint is necessary to not shift arithmetically
  auto
    r_is_max{static_cast<uint32_t>(_mm256_movemask_epi8(_mm256_cmpeq_epi8(r_reg, _mm256_max_epu8(r_reg, pd_reg))))};

// overflowed procedure
  auto max_element_positions{(~r_is_max) & static_cast<uint32_t>(valid_elements_mask)};

  if (max_element_positions == 0)
  {
    return r;

  }
  max_element_positions >>= 1;

  uint8_t max_index{0};

  uint8_t max_value{0};

  uint8_t current_index{0};

  while (max_element_positions)
  {
    if ((max_element_positions & util::bit_mask_position<int, 31>::value) && data_[1 + current_index] > max_value)
    {
      max_value = data_[1 + current_index];
      max_index = current_index;
    }
    max_element_positions >>= 1;
    ++current_index;
  }

  uint8_t max{data_[1 + max_index]};

  data_[1 + max_index] = r;

  return max;

}

[[nodiscard]] uint8_t bin::size() const
{
  return (*data_ & util::bit_mask_right<uint8_t, 5>::value);
}

[[nodiscard]] bool bin::overflowed() const
{
  return util::bit_mask_position<uint8_t, 0>::value & *data_;
}

void bin::set_overflowed() const
{
  (*data_) |= 0x80;
}

[[nodiscard]] bool bin::greater_than_max(uint8_t fp)
{
  auto r_reg{_mm256_set1_epi8(static_cast<int8_t>(fp))};
  auto pd_reg{_mm256_loadu_si256(reinterpret_cast<__m256i*>(data_))};
  return _mm256_movemask_epi8(_mm256_cmpeq_epi8(r_reg, _mm256_max_epu8(r_reg, pd_reg)));
}

void bin::increase_size()
{
  // we may ignore that the most significant bit is set, as we will never reach the point where incrementing will change that
  // as we are just manipulating the least significant 5 bits and not more here
  ++(*data_);
}
}