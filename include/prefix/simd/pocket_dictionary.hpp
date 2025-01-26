//
// Created by nils on 12/12/24.
//

#ifndef PREFIX_SIMD_POCKET_DICTIONARY_HPP
#define PREFIX_SIMD_POCKET_DICTIONARY_HPP

#include <cstdint>
#include <immintrin.h>
#include <optional>
#include <iostream>

#include "util/masks.hpp"

namespace prefix::simd
{
template<std::size_t k>
class pocket_dictionary
{
public:

  static constexpr uint8_t maximum_size{k};

  static constexpr bool query(uint8_t q, uint8_t r, uint8_t* data)
  {
    if (!size(data))
    {
      return false;
    }

    // load may be used, as we assume to be aligned
    auto pd_reg{_mm256_load_si256(reinterpret_cast<__m256i*>(data))};

    // ignore the lowest byte

    auto cmp_result{_mm256_cmpeq_epi8(pd_reg, _mm256_set1_epi8(static_cast<int8_t>(r)))};
    uint32_t
      matches{static_cast<uint32_t>((
      (_mm256_movemask_epi8(cmp_result) & (util::bit_mask_right_rt<int>::value(size(data)) << 7)) >> 7))};

    // cutdown of queries as described in the paper
    if (matches == 0)
    {
      return false;
    }

    // we need to check if the element that we have found are in the correct lists

    // we do not ignore the redundant bits + first element here by and'ing it with 0
    // because it is not necessary, as the matches mask will turn 0 before we reach there
    auto header{get_header(data)};
    uint8_t current_list{0};
    while (matches && header)
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

  static constexpr void insert(uint8_t q, uint8_t r, uint8_t* data)
  {
    auto header{get_header(data)};
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
      if (data[body_position + 7] == r)
      {
        return;
      }
      ++body_position;
      ++header_position;
      header <<= 1;
    }

    insert_into_body(body_position, r, data);
    insert_into_header(header_position, data);
  }

  static constexpr uint8_t size(uint8_t* data)
  {
    return static_cast<uint8_t>(_mm_popcnt_u64(get_header(data) & util::bit_mask_left<uint64_t, 49>::value));
  }

  static constexpr void evict_max(uint8_t* data)
  {
    const uint8_t biggest_q{get_biggest_q(data)};
    uint8_t q_copy{biggest_q};
    uint8_t body_index{0};
    auto header{get_header(data)};

    while (q_copy)
    {
      if (!(header & util::bit_mask_position<uint64_t, 0>::value))
      {
        --q_copy;
      }
      else
      {
        ++body_index;
      }
      header <<= 1;
    }


    header = get_header(data);
    uint64_t last_bits{header & util::bit_mask_right<uint64_t, 14>::value};
    header &= util::bit_mask_left<uint64_t, 49>::value;
    //                                                                        if the sum is 0 that would result in no change at all, which is never intended
    uint64_t up_to_deletion
      {header & util::bit_mask_left_rt<uint64_t>::value(static_cast<uint8_t>(biggest_q + body_index > 0 ? biggest_q
        + body_index - 1 : 0))};
    header <<= biggest_q + body_index + 1;
    header >>= biggest_q + body_index;
    header |= up_to_deletion;
    set_header(header | last_bits, data);
  }

  static constexpr void max_move_procedure(uint8_t* data)
  {
    // find the biggest q
    uint8_t q{0};
    uint8_t body_index{0};
    uint8_t current_list_size{0};
    auto header{get_header(data) & util::bit_mask_left<uint64_t, 49>::value};
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

    // set the biggest q in the redundant bits as proposed in the paper
    data[6] &= util::bit_mask_left<uint8_t, 2>::value;
    data[6] |= q;

    // at this point it is faster to search for the maximum in the last non-empty list linearely,
    // rather than doing simd

    uint8_t current_max{0};
    uint8_t current_max_index{body_index};
    for (uint8_t i{body_index}; i < size(data); ++i)
    {
      if (data[7 + i] > current_max)
      {
        current_max = data[7 + i];
        current_max_index = i;
      }
    }

    data[7 + current_max_index] = data[7 + size(data) - 1];
    data[7 + size(data) - 1] = current_max;
  }

  static constexpr uint8_t max(uint8_t* data)
  {
    return data[7 + k - 1];
  }

  static constexpr void mark_overflowed(uint8_t* data)
  {
    set_header(get_header(data) | util::bit_mask_position<uint64_t, 50>::value, data);
  }

  static constexpr bool overflowed(uint8_t* data)
  {
    return get_header(data) & util::bit_mask_position<uint64_t, 50>::value;
  }

  static constexpr uint8_t get_biggest_q(uint8_t* data)
  {
    return (data[6] & util::bit_mask_right<uint8_t, 5>::value);
  }

  static inline std::string to_string()
  {
    return "SIMD";
  }

private:


  static constexpr void set_header(uint64_t new_header, uint8_t* data)
  {
    *reinterpret_cast<uint64_t*>(data) = __builtin_bswap64(new_header);
  }

  static constexpr uint64_t get_header(uint8_t* data)
  {

    return (__builtin_bswap64((*reinterpret_cast<uint64_t*>(data))));
  }

  static constexpr void insert_into_header(uint8_t index, uint8_t* data)
  {
    uint64_t header{get_header(data)};

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

    set_header(header, data);
  }

  static constexpr void insert_into_body(uint8_t position, uint8_t r, uint8_t* data)
  {
    // shortcut to not shift anything if not necessary
    if (position == size(data))
    {
      data[position + 7] = r;
      return;
    }

    // 25 elements may be stored. (128 / 8) - 7 = 9 in the first lane and the remaining 16 in the second
    // shifting with avx-2 is only possible in one lane. Hence, we need to decide on which lane we are in
    // because then avx-2 is pretty much useless for our usecase we just use sse instructions doing the lane
    // shifting ourselves

    // we are in the second lane
    if (position >= 9)
    {
      position -= 9;
      __m128i second_lane{_mm_load_si128(reinterpret_cast<__m128i*>(data) + 1)};

      auto up_to_insertion{_mm_and_si128(second_lane, util::reg_128_bit_mask_right_rt::value(position))};

      second_lane = _mm_slli_si128(second_lane, 1);

      second_lane = _mm_and_si128(second_lane, util::reg_128_bit_mask_left_rt::value(15 - position));

      second_lane = _mm_or_si128(second_lane, up_to_insertion);
      _mm_store_si128((reinterpret_cast<__m128i*>(data) + 1), second_lane);
      data[position + 9 + 7] = r;
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
    if (size(data) >= 9)
    {
      // last key of the first lane
      shifted_out = data[15];
    }

    __m128i first_lane{_mm_load_si128(reinterpret_cast<__m128i*>(data))};
    auto up_to_insertion{_mm_and_si128(first_lane, util::reg_128_bit_mask_right_rt::value(position + 7))};

    first_lane = _mm_slli_si128(first_lane, 1);

    // 0 up to the position we want to insert to
    first_lane = _mm_and_si128(first_lane, util::reg_128_bit_mask_left_rt::value(15 - (position + 7)));

    first_lane = _mm_or_si128(first_lane, up_to_insertion);
    _mm_store_si128((reinterpret_cast<__m128i*>(data)), first_lane);
    data[position + 7] = r;

    if (shifted_out)
    {
      insert_into_body(9, *shifted_out, data);
    }
  }

};

} // namespace prefix::simd

#endif // PREFIX_SIMD_POCKET_DICTIONARY_HPP
