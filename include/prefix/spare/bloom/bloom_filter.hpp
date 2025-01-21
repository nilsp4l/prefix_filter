//
// Created by nils on 1/14/25.
//

#ifndef PREFIX_SPARE_BLOOM_BLOOM_FILTER_HPP
#define PREFIX_SPARE_BLOOM_BLOOM_FILTER_HPP

#include <cinttypes>
#include "util/hash_functions.hpp"
#include <iostream>

namespace prefix::spare
{
template<typename key_t, std::size_t size>
class bloom_filter
{
public:
  bloom_filter()
  {
    data_ = new uint8_t[size]();
  }

  bloom_filter(bloom_filter<key_t, size>&& other)
  {
    data_ = other.data_;
    other.data_ = nullptr;
  }

  bloom_filter& operator=(bloom_filter<key_t, size>&& other) noexcept
  {
    data_ = other.data_;
    other.data_ = nullptr;
    return *this;
  }

  ~bloom_filter()
  {
    delete[] data_;
  }

  void insert(std::pair<std::size_t, uint8_t> key)
  {
    for (uint32_t position : util::bloom_hash_function<size << 3>::hash(key))
    {
      data_[position / 8] |= (util::bit_mask_position_rt<uint8_t>::value(
        position & util::bit_mask_right<uint8_t, 3>::value)); // position % 8
    }

  }

  bool query(std::pair<std::size_t, uint8_t> key)
  {

    for (uint32_t position : util::bloom_hash_function<size << 3>::hash(key))
    {
      if (!(data_[position / 8] & (util::bit_mask_position_rt<uint8_t>::value(
        position & util::bit_mask_right<uint8_t, 3>::value)))) // position % 8
      {
        return false;
      }
    }

    return true;
  }

private:

  uint8_t* data_;

};
}// namespace prefix::spare
#endif //PREFIX_SPARE_BLOOM_BLOOM_FILTER_HPP
