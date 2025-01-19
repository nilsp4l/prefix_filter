//
// Created by nils on 1/14/25.
//

#ifndef PREFIX_PREFIX_FILTER_HPP
#define PREFIX_PREFIX_FILTER_HPP

#include "prefix/bin.hpp"
#include "bloom/bloom_filter.hpp"

namespace prefix
{
template<typename key_t, typename bin_t, std::size_t size>
class prefix_filter
{
public:
  prefix_filter()
  {
    // size * 32 (32 is the size of one pd)
    data_ = new uint8_t[size << 5]();
  }

  prefix_filter(prefix_filter<key_t, bin_t, size>&& other) noexcept
  {
    {
      data_ = other.data_;
      other.data_ = nullptr;
      bloom_ = std::move(other.bloom_);
    }
  }

  prefix_filter& operator=(prefix_filter<key_t, bin_t, size>&& other) noexcept
  {

    data_ = other.data_;
    other.data_ = nullptr;
    bloom_ = std::move(other.bloom_);
    return *this;
  }

  ~prefix_filter()
  {
    delete[] data_;
  }

  void insert(key_t key)
  {
    auto fp{util::prefix_fingerprint<key_t, size>::fp(key)};
    auto insert_return{bin_t::insert(fp.second, data_ + (fp.first << 5))};
    if (insert_return)
    {
      bloom_.insert(fp);
    }
  }

  bool query(key_t key)
  {
    auto fp{util::prefix_fingerprint<key_t, size>::fp(key)};
    if (!bin_t::overflowed(data_ + (fp.first << 5)))
    {
      return bin_t::query(fp.second, data_ + (fp.first << 5));
    }

    if (bin_t::greater_than_max(fp.second, data_ + (fp.first << 5)))
    {
      return bloom_.query(fp);
    }

    return bin_t::query(fp.second, data_ + (fp.first << 5));

  }

private:


  uint8_t* data_{nullptr};
  bloom_filter<uint16_t, 10485706> bloom_;
};
}

#endif //PREFIX_PREFIX_FILTER_HPP
