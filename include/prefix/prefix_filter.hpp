//
// Created by nils on 1/14/25.
//

#ifndef PREFIX_PREFIX_FILTER_HPP
#define PREFIX_PREFIX_FILTER_HPP

#include "prefix/bin.hpp"
#include "prefix/spare/bloom/bloom_filter.hpp"
#include <cstdlib>

namespace prefix
{
template<typename key_t, typename bin_t, typename spare_t, std::size_t size>
class prefix_filter
{
public:
  prefix_filter()
  {
    // size * 32 (32 is the size of one pd)
    data_ = reinterpret_cast<uint8_t*>(std::aligned_alloc(32, size << 5));
    for (std::size_t i{0}; i < (size << 5); ++i)
    {
      data_[i] = 0;
    }
  }

  prefix_filter(prefix_filter<key_t, bin_t, spare_t, size>&& other) noexcept
  {
    {
      data_ = other.data_;
      other.data_ = nullptr;
      spare_ = std::move(other.spare_);
    }
  }

  ~prefix_filter()
  {
    std::free(data_);
  }

  prefix_filter& operator=(prefix_filter<key_t, bin_t, spare_t, size>&& other) noexcept
  {

    data_ = other.data_;
    other.data_ = nullptr;
    spare_ = std::move(other.spare_);
    return *this;
  }

  void insert(key_t key)
  {
    auto fp{util::prefix_fingerprint<key_t, size>::fp(key)};
    auto insert_return{bin_t::insert(fp.second, data_ + (fp.first << 5))};


    if (insert_return)
    {
      spare_.insert({fp.first, *insert_return});
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
      return spare_.query(fp);
    }

    return bin_t::query(fp.second, data_ + (fp.first << 5));

  }

  // for benchmark
  static constexpr inline std::size_t get_byte_size()
  {
    return size * bin_t::get_byte_size() + spare_t::get_byte_size();
  }

  static constexpr inline std::size_t get_spare_byte_size()
  {
    return spare_t::get_byte_size();
  }

  static inline std::string to_string()
  {
    return bin_t::to_string() + "-Prefix-Filter[" + spare_t::to_string() + "]";
  }

private:

  uint8_t* data_{nullptr};
  spare_t spare_;
};
}

#endif //PREFIX_PREFIX_FILTER_HPP
