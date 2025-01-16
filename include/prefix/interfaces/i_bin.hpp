//
// Created by nils on 11/15/24.
//

#ifndef INCLUDE_PREFIX_INTERFACES_I_BIN_HPP
#define INCLUDE_PREFIX_INTERFACES_I_BIN_HPP

#include <cstdint>
#include <optional>

namespace prefix::interfaces
{
class i_bin
{
public:
  virtual ~i_bin() = default;

  virtual uint8_t& operator[](uint8_t index) const = 0;

  [[nodiscard]] virtual bool query(uint8_t fp) const = 0;

  virtual std::optional<uint8_t> insert(uint8_t fp) = 0;

  [[nodiscard]] virtual uint8_t size() const = 0;

  [[nodiscard]] virtual bool overflowed() const = 0;

  [[nodiscard]] virtual bool smaller_than_max(uint8_t fp) = 0;

};
}// namespace prefix::interfaces

#endif //INCLUDE_PREFIX_INTERFACES_I_BIN_HPP
