//
// Created by nils on 11/15/24.
//

#ifndef INCLUDE_PREFIX_NON_SIMD_BIN_H
#define INCLUDE_PREFIX_NON_SIMD_BIN_H

#include "prefix/interfaces/i_bin.hpp"

namespace prefix::non_simd
{
  template<uint8_t Q>
  class bin : public interfaces::i_bin
  {
  public:
    bin()
    {
      static_assert(((Q - 1) & 0x4) == 0, "Q - 1 must be a multiple of 8");
      pd = new uint8_t[512];
    }

    ~bin() override
    {
      delete[] pd;
    }

    uint8_t operator[](uint8_t index) const
    {
      return pd[((Q - 1) >> 3) + index];
    }


    [[nodiscard]] constexpr bool query(uint8_t key) const override
    {
      return true;
    }

    constexpr void insert(uint8_t key) override {}

  private:
    uint8_t *pd;
  };
} // namespace prefix::non_simd


#endif //INCLUDE_PREFIX_NON_SIMD_BIN_H
