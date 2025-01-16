//
// Created by nils on 1/14/25.
//

#ifndef BLOOM_BLOOM_FILTER_HPP
#define BLOOM_BLOOM_FILTER_HPP

#include <cinttypes>
#include "util/hash_functions.hpp"
#include <iostream>

template<typename key_t>
class bloom_filter
{
public:
  bloom_filter()
  {
    data_ = new uint8_t[size]();
  }

  ~bloom_filter()
  {
    delete[] data_;
  }

  void insert(key_t key)
  {
    for (uint32_t position : util::bloom_hash_function<key_t, size << 3>::hash(key))
    {
      data_[position / 8] |= (util::bit_mask_position_rt<uint8_t>::value(
        position & util::bit_mask_right<uint8_t, 3>::value)); // position % 8
    }
  }

  bool query(key_t key)
  {
    for (uint32_t position : util::bloom_hash_function<key_t, size << 3>::hash(key))
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
  static constexpr uint32_t size{512};
};
#endif //BLOOM_BLOOM_FILTER_HPP
