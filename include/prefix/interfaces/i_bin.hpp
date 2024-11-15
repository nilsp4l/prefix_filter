//
// Created by nils on 11/15/24.
//

#ifndef I_BIN_HPP
#define I_BIN_HPP

#include <cstdint>

namespace prefix::interfaces
{
  class i_bin
  {
  public:
    virtual ~i_bin() = default;

    constexpr virtual bool query(uint8_t key) const = 0;

    constexpr virtual void insert(uint8_t key) = 0;
  };
}// namespace prefix::interfaces

#endif //I_BIN_HPP
