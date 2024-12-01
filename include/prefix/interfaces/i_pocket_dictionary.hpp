//
// Created by nils on 11/24/24.
//

#ifndef INCLUDE_PREFIX_INTERFACES_I_POCKET_DICTIONARY_HPP
#define INCLUDE_PREFIX_INTERFACES_I_POCKET_DICTIONARY_HPP

#include <cinttypes>

namespace prefix::interfaces
{
class i_pocket_dictionary
{
public:
  virtual ~i_pocket_dictionary() = default;

  virtual uint8_t& operator[](uint8_t index) const = 0;

  [[nodiscard]] constexpr virtual bool query(uint8_t q, uint8_t r) const = 0;

  constexpr virtual void insert(uint8_t q, uint8_t r) = 0;

  [[nodiscard]] virtual constexpr uint8_t size() const = 0;

  virtual constexpr void evict_max() = 0;

  virtual constexpr void max_move_procedure() = 0;

  // retrieves the header if the dictionary has already overflowed, which then is required to be
  // in the last position of the body. otherwise undefined behavior
  [[nodiscard]] virtual constexpr uint8_t max() const = 0;

  virtual constexpr void mark_overflowed() = 0;

  [[nodiscard]] virtual constexpr bool overflowed() const = 0;

};
}// namespace prefix::interfaces
#endif //INCLUDE_PREFIX_INTERFACES_I_POCKET_DICTIONARY_HPP
