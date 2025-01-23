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

  filter& operator=(filter<uint64_t, size>&& other) noexcept
  {

    data_ = other.data_;
    other.data_ = nullptr;
    return *this;
  }

  void insert(std::pair<std::size_t, key_t> key)
  {


    if (bin::size(data_ + ((key.first << 5) % size)) <= bin::maximum_size)
    {
      bin::insert(key.second, data_ + ((key.first << 5) % size));
    }
    else
    {
      std::cout << "overflowed" << std::endl;
    }

  }

  bool query(std::pair<std::size_t, key_t> key)
  {
    //std::cout << std::to_string(counter++) << std::endl;
    // it is unlikely to ever happen, but if it does, we must return true to guarantee no false negatives
    if (bin::size(data_ + ((key.first << 5) % size)) == bin::maximum_size)
    {
      return true;
    }

    return bin::query(key.second, data_ + ((key.first << 5) % size));

  }

private:
  uint8_t* data_{nullptr};
  std::size_t counter{0};
};
}

#endif //PREFIX_SPARE_PREFIX_ADAPTED_HPP
