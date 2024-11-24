//
// Created by nils on 11/15/24.
//

#ifndef INCLUDE_PREFIX_INTERFACES_I_BIN_HPP
#define INCLUDE_PREFIX_INTERFACES_I_BIN_HPP

#include <cstdint>

namespace prefix::interfaces
{
class i_bin
{
public:
  virtual ~i_bin() = default;

  virtual uint8_t& operator[](uint8_t index) const = 0;

  [[nodiscard]] constexpr virtual bool query(uint8_t fp) const = 0;

  constexpr virtual void insert(uint8_t fp) = 0;

  virtual constexpr uint8_t size() const = 0;
};
}// namespace prefix::interfaces

#endif //INCLUDE_PREFIX_INTERFACES_I_BIN_HPP
