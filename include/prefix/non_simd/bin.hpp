//
// Created by nils on 11/15/24.
//

#ifndef INCLUDE_PREFIX_NON_SIMD_BIN_H
#define INCLUDE_PREFIX_NON_SIMD_BIN_H

#include <bit>
#include <array>
#include "prefix/interfaces/i_bin.hpp"
#include "prefix/util/masks.hpp"

namespace prefix::non_simd
{

   /// this is the naive implementation used mainly for comparison and base of the optimized ones, as
   /// it is much easier to understand.
   /// !Not optimized at all!
   template <uint8_t k>
   class bin : public interfaces::i_bin
   {
   public:
      bin()
      {
         static_assert(k <= 25);
         // initialized with 0, which is important for the header to be initialized with no elements in it
         pd = new uint8_t[64]();
      }

      ~bin() override
      {
         delete[] pd;
      }

      uint8_t &operator[](uint8_t index) const
      {
         return pd[8 + index];
      }

      [[nodiscard]] constexpr bool query(const uint8_t q, const uint8_t r) const override
      {
         auto index{get_list_index(q)};
         uint8_t list_size{get_list_size(index[0] + index[1])};
         for (uint8_t i{index[1]}; i < index[1] + list_size; ++i)
         {
            if (this->operator[](i) == r)
            {
               return true;
            }
         }
         return false;
      }

      constexpr void insert(const uint8_t q, const uint8_t r) override
      {
         if (q >= k)
         {
            return;
         }

         // already k elements in pd
         if (size() == k)
         {
            overflowed_procedure();
            return;
         }
         auto list_index{get_list_index(q)};
         insert_into_body(list_index[1] + get_list_size(list_index[0] + list_index[1]), r);
         insert_into_header(list_index[0] + list_index[1]);
      }

      constexpr uint8_t size()
      {
         return static_cast<uint8_t>(std::popcount(*reinterpret_cast<uint64_t *>(pd)));
      }

   private:
      constexpr void overflowed_procedure() {}

      constexpr void insert_into_body(uint8_t index, int8_t r)
      {

         for (uint8_t i{index}; i < k - 1; ++i)
         {
            this->operator[](i + 1) = this->operator[](i);
         }
         this->operator[](index) = r;
      }

      constexpr void insert_into_header(uint8_t index)
      {
         auto *header{reinterpret_cast<uint64_t *>(pd)};
         uint64_t up_to_insertion{(util::bit_mask_left_rt<uint64_t>::value(index) & *header)};
         *header <<= index;
         *header >>= index + 1;
         *header |= up_to_insertion;
         *header |= util::bit_mask_position_rt<uint64_t>::value(index);
      }

      [[nodiscard]] constexpr uint8_t get_list_size(uint8_t list_position) const
      {
         uint64_t header = *reinterpret_cast<uint64_t *>(pd);
         header <<= list_position;
         const uint64_t first_bit_mask{util::bit_mask_position<uint64_t, 0>::value};
         uint8_t count{0};
         while (header & first_bit_mask)
         {
            ++count;
            header <<= 1;
         }
         return count;
      }

      [[nodiscard]] constexpr std::array<uint8_t, 2> get_list_index(uint8_t q) const
      {
         uint64_t header{*reinterpret_cast<uint64_t *>(pd)};

         uint8_t remaining{k};
         uint8_t zero_count{0};
         constexpr uint8_t first_bit_mask{util::bit_mask_position<uint8_t, 0>::value}; // <| 0b1000 0000
         while (remaining > 0)
         {
            if ((header & first_bit_mask))
            {
               --remaining;
            }
            // we are in the required list
            else if (zero_count == q)
            {
               return {zero_count, static_cast<uint8_t>(k - remaining)};
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

#endif // INCLUDE_PREFIX_NON_SIMD_BIN_H
