//
// Created by nils on 1/21/25.
//

#ifndef PREFIX_SPARE_PREFIX_ADAPTED_HPP
#define PREFIX_SPARE_PREFIX_ADAPTED_HPP

#include <cinttypes>
#include <cstdlib>

#include "util/hash_functions.hpp"
#include "prefix/spare/prefix_adapted/bin.hpp"

namespace prefix::spare::prefix_adapted
{
template<typename key_t, std::size_t size>
class filter
{
public:
  filter()
  {
    // size * 32 (32 is the size of one pd)
    data_ = reinterpret_cast<uint16_t*>(std::aligned_alloc(32, size << 5));
    for (std::size_t i{0}; i < (size << 4); ++i)
    {
      data_[i] = 0;
    }
  }

  ~filter()
  {
    std::free(data_);
  }

  filter(filter<key_t, size>&& other) noexcept
  {

    data_ = other.data_;
    other.data_ = nullptr;

  }

  filter& operator=(filter<uint64_t, size>&& other) noexcept
  {

    data_ = other.data_;
    other.data_ = nullptr;
    return *this;
  }

  void insert(std::pair<std::size_t, key_t> key)
  {


    uint64_t new_key{(key.first << 8) | key.second};
    auto fp{util::prefix_fingerprint<uint64_t, size>::fp(new_key)};

    if (bin::overflowed(data_ + ((fp.first << 4))))
    {
      return;
    }

    // insert into already full filter overflows the filter
    if (bin::size(data_ + ((fp.first << 4))) == bin::maximum_size)
    {
      bin::set_overflowed(data_ + ((fp.first << 4)));
      return;
    }

    if (bin::size(data_ + ((fp.first << 4))) < bin::maximum_size)
    {
      bin::insert(fp.second, data_ + ((fp.first << 4)));
    }

  }

  bool query(std::pair<std::size_t, key_t> key)
  {
    uint64_t new_key{(key.first << 8) | key.second};
    auto fp{util::prefix_fingerprint<uint64_t, size>::fp(new_key)};
    // it is unlikely to ever happen, but if it does, we must return true to guarantee no false negatives
    if (bin::overflowed(data_ + ((fp.first << 4))))
    {
      return true;
    }

    return bin::query(fp.second, data_ + ((fp.first << 4)));

  }

  static constexpr inline std::size_t get_byte_size()
  {
    return size * bin::get_byte_size();
  }

  static inline std::string to_string()
  {
    return "Prefix-Adapted";
  }

private:
  uint16_t* data_{nullptr};
  std::size_t counter{0};
};
}

#endif //PREFIX_SPARE_PREFIX_ADAPTED_HPP
