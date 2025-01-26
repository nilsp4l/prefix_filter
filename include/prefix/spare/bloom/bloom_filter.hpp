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

// see https://stackoverflow.com/questions/658439/how-many-hash-functions-does-my-bloom-filter-need
template<std::size_t elements_to_store>
constexpr static std::array<std::size_t, 2> calculate_bloom_size()
{
  auto n{static_cast<double>(elements_to_store)};

  // ln(0.001) as 0.1 % is the desired false positive rate
  double ln_p{-6.90775527898};

  // ln(2)
  double ln_2{0.69314718056};

  auto m{static_cast<std::size_t>(-n * ln_p / (ln_2 * ln_2))};
  auto k{static_cast<size_t>(static_cast<double>(m) / n * ln_2)};
  return {m, k};

};

template<typename key_t, std::size_t size, std::size_t no_hashes>
class bloom_filter
{
public:
  bloom_filter()
  {
    data_ = new uint8_t[size]();
  }

  bloom_filter(bloom_filter<key_t, size, no_hashes>&& other)
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

  void insert(std::pair<std::size_t, uint8_t> key)
  {
    for (uint64_t position : util::bloom_hash_function<(size << 3), no_hashes>::hash(key))
    {
      data_[position / 8] |= (util::bit_mask_position_rt<uint8_t>::value(position % 8)); // position % 8
    }

  }

  bool query(std::pair<std::size_t, uint8_t> key)
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

private:

  uint8_t* data_;

};
}// namespace prefix::spare
#endif //PREFIX_SPARE_BLOOM_BLOOM_FILTER_HPP
