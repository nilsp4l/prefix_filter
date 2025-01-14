//
// Created by nils on 1/14/25.
//

#ifndef PREFIX_PREFIX_FILTER_HPP
#define PREFIX_PREFIX_FILTER_HPP

#include "prefix/bin.hpp"
#include "bloom/bloom_filter.hpp"

namespace prefix
{
template<typename key_t, std::size_t size>
class prefix_filter
{
public:
  prefix_filter()
  {
    bins_ = new bin<25, simd::pocket_dictionary<25>>[size]();
  }

  ~prefix_filter()
  {
    delete[] bins_;
  }

  void insert(key_t key)
  {
    auto fp{util::prefix_fingerprint<key_t, size>::fp(key)};
    auto insert_return{bins_[fp.first].insert(fp.second)};
    if (insert_return)
    {
      bloom_.insert(create_bloom_key(fp));
    }
  }

  bool query(key_t key)
  {
    auto fp{util::prefix_fingerprint<key_t, size>::fp(key)};
    if (!bins_[fp.first].overflowed())
    {
      return bins_[fp.first].query(fp.second);
    }

    if (bins_[fp.first].smaller_than_max(fp.second))
    {
      return bloom_.query(create_bloom_key(fp));
    }

    return false;
  }

private:

  uint16_t create_bloom_key(std::pair<std::size_t, uint8_t> fp)
  {
    uint16_t upper_bits{static_cast<uint16_t>(static_cast<uint8_t>(fp.first % size) << 8)};
    uint16_t lower_bits{static_cast<uint16_t>(fp.second)};
    return (upper_bits | lower_bits);
  }

  bin<25, simd::pocket_dictionary<25>>* bins_;
  bloom_filter<uint16_t> bloom_;
};
}

#endif //PREFIX_PREFIX_FILTER_HPP
