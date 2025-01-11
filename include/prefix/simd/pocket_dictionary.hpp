//
// Created by nils on 12/12/24.
//

#ifndef PREFIX_SIMD_POCKET_DICTIONARY_HPP
#define PREFIX_SIMD_POCKET_DICTIONARY_HPP

#include <cstdint>
#include <immintrin.h>
#include <optional>
#include <iostream>

#include "prefix/interfaces/i_pocket_dictionary.hpp"
#include "util/masks.hpp"

namespace prefix::simd
{
  template <std::size_t k>
  class pocket_dictionary : public interfaces::i_pocket_dictionary
  {
  public:
    pocket_dictionary()
    {
      static_assert(k <= 25);
      data_ = new uint8_t[32]();
    }

    ~pocket_dictionary() override
    {
      delete[] data_;
    }

    uint8_t &operator[](const uint8_t index) const override
    {
      return data_[7 + index];
    }

    [[nodiscard]] constexpr bool query(uint8_t q, uint8_t r) const override
    {
      if (!size())
      {
        return false;
      }

      // loadu may be used, as we never cross cache lines
      auto pd_reg{_mm256_loadu_si256(reinterpret_cast<__m256i *>(data_))};

      // ignore the lowest byte

      auto cmp_result{_mm256_cmpeq_epi8(pd_reg, _mm256_set1_epi8(static_cast<int8_t>(r)))};
      auto matches{((_mm256_movemask_epi8(cmp_result) & (util::bit_mask_right_rt<int>::value(size()) << 7)) >> 7)};

      // cutdown of queries as described in the paper
      if (matches == 0)
      {
        return false;
      }

      // we need to check if the element that we have found are in the correct lists

      // we do not ignore the redundant bits + first element here by and'ing it with 0
      // because it is not necessary, as the matches mask will turn 0 before we reach there
      auto header{get_header()};
      uint8_t current_list{0};
      while (matches)
      {
        if (!(header & util::bit_mask_left<uint64_t, 0>::value))
        {
          ++current_list;
          header <<= 1;
          continue;
        }

        if ((matches & util::bit_mask_right<int, 1>::value) && current_list == q)
        {
          return true;
        }

        matches >>= 1;
        header <<= 1;
      }

      return false;
    }

    constexpr void insert(uint8_t q, uint8_t r) override
    {
      auto header{get_header()};
      uint8_t q_cpy{q};
      uint8_t body_position{0};
      uint8_t header_position{0};

      // find the list
      while (q_cpy)
      {
        if (header & util::bit_mask_position<uint64_t, 0>::value)
        {
          ++body_position;
          ++header_position;
        }
        else
        {
          ++header_position;
          --q_cpy;
        }
        header <<= 1;
      }

      // find the end of the list
      while (header & util::bit_mask_position<uint64_t, 0>::value)
      {
        if (data_[body_position + 7] == r)
        {
          return;
        }
        ++body_position;
        ++header_position;
        header <<= 1;
      }

      insert_into_body(body_position, r);
      insert_into_header(header_position);
    }

    [[nodiscard]] constexpr uint8_t size() const override
    {
      return static_cast<uint8_t>(std::popcount(get_header() & util::bit_mask_left<uint64_t, 49>::value));
    }

    constexpr void evict_max() override
    {
      const uint8_t biggest_q{get_biggest_q()};
      uint8_t q_copy{biggest_q};
      uint8_t body_index{0};
      auto header{get_header()};
      while(q_copy)
      {
        if(!(header & util::bit_mask_position<uint64_t, 0>::value))
        {
          --q_copy;
        }
        else
        {
          ++body_index;
        }
        header <<= 1;
      }
      

      header = get_header();
      uint64_t last_bits{header & util::bit_mask_right<uint64_t, 14>::value};
      header &= util::bit_mask_left<uint64_t, 49>::value;
      uint64_t up_to_deletion{header & util::bit_mask_left_rt<uint64_t>::value(static_cast<uint8_t>(biggest_q + body_index - 1))};
      header <<= biggest_q + body_index + 1;
      header >>= biggest_q + body_index;
      header |= up_to_deletion;
      set_header(header | last_bits);
    }

    constexpr void max_move_procedure() override
    {
      // find the biggest q
      uint8_t q{0};
      uint8_t body_index{0};
      uint8_t current_list_size{0};
      auto header{get_header() & util::bit_mask_left<uint64_t, 49>::value};
      while (header)
      {
        if (!(header & util::bit_mask_left<uint64_t, 0>::value))
        {
          body_index += current_list_size;
          current_list_size = 0;
          ++q;
        }
        else
        {
          ++current_list_size;
        }
        header <<= 1;
      }

      // set the biggest q in the redundant bits as proposed in the paper for whatever reason
      uint64_t header_reg{get_header() & util::bit_mask_left<uint64_t, 55>::value};
      uint64_t header_up_to_q{get_header() & util::bit_mask_right<uint64_t, 8>::value};
      uint64_t biggest_q_position{~(util::bit_mask_left<uint64_t, 4>::value >> 51)}; // keep everything except for the biggest_q_position to clear it
      header_reg &= biggest_q_position;
      uint64_t new_biggest_q_64{static_cast<uint64_t>(q)};
      new_biggest_q_64 <<= 8;
      header_reg |= new_biggest_q_64;
      set_header(header_reg | header_up_to_q);

      // at this point it is faster to search for the maximum in the last non-empty list linearely,
      // rather than doing simd

      uint8_t current_max{0};
      uint8_t current_max_index{body_index};
      for (uint8_t i{body_index}; i < size(); ++i)
      {
        if (data_[7 + i] > current_max)
        {
          current_max = data_[7 + i];
          current_max_index = i;
        }
      }

      data_[7 + current_max_index] = data_[7 + size() - 1];
      data_[7 + size() - 1] = current_max;
    }

    [[nodiscard]] constexpr uint8_t max() const override
    {
      return data_[7 + k - 1];
    }

    constexpr void mark_overflowed() override
    {
      set_header(get_header() | util::bit_mask_position<uint64_t, 50>::value);
    }

    [[nodiscard]] constexpr bool overflowed() const override
    {
      return get_header() & util::bit_mask_position<uint64_t, 50>::value;
    }

  private:
    constexpr uint8_t get_biggest_q()
    {
      uint64_t header_reg{get_header() & util::bit_mask_left<uint64_t, 55>::value};
      header_reg >>= 8;                                        // move the position of q into the least significant 5 bits
      header_reg &= ~util::bit_mask_left<uint64_t, 58>::value; // 0 everything except for the least significant 5 bits
      return static_cast<uint8_t>(header_reg);                 // ignore everything except for the first four bits
    }

    constexpr void set_header(uint64_t new_header)
    {
      *reinterpret_cast<uint64_t *>(data_) = __builtin_bswap64(new_header);
    }

    [[nodiscard]] constexpr uint64_t get_header() const
    {

      return (__builtin_bswap64((*reinterpret_cast<uint64_t *>(data_))));
    }

    constexpr void insert_into_header(uint8_t index)
    {
      uint64_t header{get_header()};

      uint64_t up_to_insertion{(util::bit_mask_left_rt<uint64_t>::value(index) & header)};
      // element in last position + redundant bits
      uint64_t not_header_bits{(util::bit_mask_right<uint64_t, 14>::value) & header};

      header <<= index;
      header >>= index + 1;

      // 0 the last bits to or them afterwards
      header >>= 14;
      header <<= 14;

      header |= up_to_insertion;
      header |= not_header_bits;
      header |= util::bit_mask_position_rt<uint64_t>::value(index);

      set_header(header);
    }

    constexpr void insert_into_body(uint8_t position, uint8_t r)
    {
      // shortcut to not shift anything if not necessary
      if (position == size())
      {
        data_[position + 7] = r;
        return;
      }

      // 25 elements may be stored. (128 / 8) - 7 = 9 in the first lane and the remaining 16 in the second
      // shifting with avx-2 is only possible in one lane. Hence, we need to decide on which lane we are in
      // because then avx-2 is pretty much useless for our usecase we just use sse instructions doing the lane
      // shifting ourself

      // we are in the second lane
      if (position >= 9)
      {
        position -= 9;
        __m128i second_lane{_mm_load_si128(reinterpret_cast<__m128i *>(data_) + 1)};

        auto up_to_insertion{_mm_and_si128(second_lane, util::reg_128_bit_mask_right_rt::value(position))};

        second_lane = _mm_slli_si128(second_lane, 1);

        second_lane = _mm_and_si128(second_lane, util::reg_128_bit_mask_left_rt::value(15 - position));

        second_lane = _mm_or_si128(second_lane, up_to_insertion);
        _mm_store_si128((reinterpret_cast<__m128i *>(data_) + 1), second_lane);
        data_[position + 9 + 7] = r;
        return;
      }

      // we are in the first lane

      // if we shift something out of the lane we need to put it in the other lane.
      // we need to store it before we shift it out.
      // then the optional will give us information if something is shifted out later
      // with the regarding key we need to insert in the second lane
      std::optional<uint8_t> shifted_out;

      // we need to go here for size() == 9 as well, because the header is not updated on the size we will reach after insertion
      // yet, then it will be > 9 and we'll need to shift out
      if (size() >= 9)
      {
        // last key of the first lane
        shifted_out = data_[15];
      }

      __m128i first_lane{_mm_load_si128(reinterpret_cast<__m128i *>(data_))};
      auto up_to_insertion{_mm_and_si128(first_lane, util::reg_128_bit_mask_right_rt::value(position + 7))};

      first_lane = _mm_slli_si128(first_lane, 1);

      // 0 up to the position we want to insert to
      first_lane = _mm_and_si128(first_lane, util::reg_128_bit_mask_left_rt::value(15 - (position + 7)));

      first_lane = _mm_or_si128(first_lane, up_to_insertion);
      _mm_store_si128((reinterpret_cast<__m128i *>(data_)), first_lane);
      data_[position + 7] = r;

      if (shifted_out)
      {
        insert_into_body(9, *shifted_out);
      }
    }

    uint8_t *data_;
  };

} // namespace prefix::simd

#endif // PREFIX_SIMD_POCKET_DICTIONARY_HPP
