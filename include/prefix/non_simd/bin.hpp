//
// Created by nils on 11/15/24.
//

#ifndef INCLUDE_PREFIX_NON_SIMD_BIN_H
#define INCLUDE_PREFIX_NON_SIMD_BIN_H

#include <bit>
#include "prefix/interfaces/i_bin.hpp"
#include "prefix/util/masks.hpp"


namespace prefix::non_simd
{

  /// this is the naive implementation used mainly for comparison and base of the optimized ones, as
  /// it is much easier to understand.
  /// !Not optimized at all!
  template<uint8_t k>
  class bin : public interfaces::i_bin
  {
  public:
    bin()
    {
      static_assert(k <= 25);

      // initialized with 0, which is important
      pd = new uint8_t[64]();
    }

    ~bin() override
    {
      delete[] pd;
    }

    uint8_t& operator[](uint8_t index) const
    {
      return pd[8 + index];
    }


    [[nodiscard]] constexpr bool query(const uint8_t q, const uint8_t r) const override
    {
      uint8_t index{get_list_index(q)};
      return true;
    }

    constexpr void insert(const uint8_t q, const uint8_t r) override
    {
      if(q >= k)
      {
        return;
      }

      auto header = static_cast<uint64_t>(*pd);

      // already k elements in pd
      if (std::popcount(header) == k)
      {
        overflowed_procedure();
        return;
      }

      //insert_into_list(get_list_index(q) + count_to_list(q), r);

    }

  private:
    constexpr void overflowed_procedure() {}
    constexpr void insert_into_list(uint8_t index, int8_t r)
    {
      for(uint8_t i{index}; i < k - 1; ++i)
      {
        this->operator[](i + 1) = this->operator[](i);
      }
    }

    constexpr uint8_t get_list_size(uint8_t list_position)
    {
      uint8_t header = 1;

      const uint8_t first_bit_mask{0x80};
      uint8_t count{0};
      while(header & first_bit_mask)
      {
        ++count;
        header <<= 1;
      }
      return count;
    }

    constexpr uint8_t get_list_index(uint8_t q) const
    {
      uint8_t header{1};

      uint8_t remaining{k};
      uint8_t zero_count{0};
      constexpr uint8_t first_bit_mask{util::bit_mask<uint8_t, 0>::value}; // <| 0b1000 0000
      while(remaining > 0)
      {
        if((header & first_bit_mask) == 1)
        {
          --remaining;
        }
        // we are in the required list
        else if(zero_count == q)
        {
          return zero_count + (k - remaining);
        }
        else
        {
          ++zero_count;
        }
        header <<= 1;
      }

      __builtin_unreachable();
    }

    uint8_t *pd;
  };
} // namespace prefix::non_simd


#endif //INCLUDE_PREFIX_NON_SIMD_BIN_H
