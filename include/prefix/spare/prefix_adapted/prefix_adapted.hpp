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
    data_ = reinterpret_cast<uint8_t*>(std::aligned_alloc(16, size << 5));
    for (std::size_t i{0}; i < (size << 5); ++i)
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
    {
      data_ = other.data_;
      other.data_ = nullptr;
    }
  }

  filter& operator=(filter<key_t, size>&& other) noexcept
  {

    data_ = other.data_;
    other.data_ = nullptr;
    return *this;
  }

  void insert(key_t key)
  {

    auto fp{util::prefix_fingerprint<key_t, size>::fp(key)};

    if (bin::size(data_ + (fp.first << 5)) <= bin::maximum_size)
    {
      bin::insert(fp.second, data_ + (fp.first << 5));
    }

  }

  bool query(key_t key)
  {
    auto fp{util::prefix_fingerprint<key_t, size>::fp(key)};

    return bin::query(fp.second, data_ + (fp.first << 5));

  }

private:
  uint8_t* data_{nullptr};
};
}

#endif //PREFIX_SPARE_PREFIX_ADAPTED_HPP
