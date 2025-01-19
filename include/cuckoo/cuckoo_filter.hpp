//
// Created by nils on 12/8/24.
//

#ifndef CUCKOO_CUCKOO_FILTER_HPP
#define CUCKOO_CUCKOO_FILTER_HPP
#include <cstdint>
#include "util/hash_functions.hpp"

template<std::size_t size, std::size_t max_loop>
class cuckoo_filter
{
  cuckoo_filter()
  {
    // for sake of simplicity we enforce buckets of four
    static_assert(size & 0x3);
    data_ = new uint16_t[size]();
  }

  bool query(uint64_t key)
  {
    /*
    if(data_[util::cuckoo_hash<(size >> 2)>::hash(key)] == util::cuckoo_fingerprint::fp(key)
    || data_[util::cuckoo_hash<(size >> 2)>::hash(key) ^ ]^)
  */
    return true;
  }

private:

  uint16_t* data_;
};


#endif //CUCKOO_CUCKOO_FILTER_HPP
