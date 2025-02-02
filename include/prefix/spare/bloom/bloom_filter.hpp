//
// Created by nils on 1/14/25.
//

#ifndef PREFIX_SPARE_BLOOM_BLOOM_FILTER_HPP
#define PREFIX_SPARE_BLOOM_BLOOM_FILTER_HPP

#include <cinttypes>
#include "util/hash_functions.hpp"

namespace prefix::spare::bloom
{


template<typename key_t, std::size_t size, std::size_t no_hashes>
class bloom_filter
{
public:
  bloom_filter()
  {
    data_ = new uint8_t[size]();
  }

  bloom_filter(bloom_filter<key_t, size, no_hashes>&& other) noexcept
  {
    data_ = other.data_;
    other.data_ = nullptr;
  }

  bloom_filter& operator=(bloom_filter<key_t, size, no_hashes>&& other) noexcept
  {
    data_ = other.data_;
    other.data_ = nullptr;
    return *this;
  }

  ~bloom_filter()
  {
    delete[] data_;
  }

  void insert(key_t key)
  {
    for (uint64_t position : util::bloom_hash_function<(size << 3), no_hashes>::hash(key))
    {
      data_[position / 8] |= (util::bit_mask_position_rt<uint8_t>::value(position % 8)); // position % 8
    }

  }

  bool query(key_t key)
  {

    for (uint64_t position : util::bloom_hash_function<(size << 3), no_hashes>::hash(key))
    {
      if (!(data_[position / 8] & (util::bit_mask_position_rt<uint8_t>::value(position % 8)))) // position % 8
      {
        return false;
      }
    }

    return true;
  }

  static constexpr inline std::size_t get_byte_size()
  {
    return size;
  }

  static inline std::string to_string()
  {
    return "Bloom";
  }

  // may be ignored, just there to make fp rate benchmarking generic
  static constexpr inline std::size_t get_spare_byte_size()
  {
    return 0;
  }

private:

  uint8_t* data_;

};
}// namespace prefix::spare
#endif //PREFIX_SPARE_BLOOM_BLOOM_FILTER_HPP
