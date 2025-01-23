//
// Created by nils on 11/24/24.
//

#ifndef INCLUDE_PREFIX_NON_SIMD_POCKET_DICTIONARY_HPP
#define INCLUDE_PREFIX_NON_SIMD_POCKET_DICTIONARY_HPP

#include "util/masks.hpp"

#include <array>
#include <cstdint>
#include <cassert>

namespace prefix::non_simd
{
/// this is the naive implementation used mainly for comparison and base of the optimized ones, as
/// it is much easier to understand.
/// !Not optimized at all!
template<uint8_t k>
class pocket_dictionary
{
public:

  static constexpr uint8_t maximum_size{k};

  static constexpr bool query(const uint8_t q, const uint8_t r, uint8_t* data)
  {
    auto index{get_list_index(q, data)};
    uint8_t list_size{get_list_size(index[0] + index[1], data)};
    for (uint8_t i{index[1]}; i < index[1] + list_size; ++i)
    {
      if (data[7 + i] == r)
      {
        return true;
      }
    }
    return false;
  }

  static constexpr void insert(const uint8_t q, const uint8_t r, uint8_t* data)
  {

    // q is greater than the amount of lists there is or already k elements in data_
    if (q >= k || size(data) == k + 1)
    {
      return;
    }

    auto list_index{get_list_index(q, data)};
    auto list_size{get_list_size(list_index[0] + list_index[1], data)};

    // element is inserted because it wasn't already there
    if (insert_into_body(list_index[1], r, list_size, data))
    {
      insert_into_header(static_cast<uint8_t>(list_index[0] + list_index[1] + list_size), data);
    }

  }

  static constexpr uint8_t size(uint8_t* data)
  {
    auto header{get_header(data)};
    return static_cast<uint8_t>(_mm_popcnt_u64(header));
  }

  static constexpr void max_move_procedure(uint8_t* data)
  {
    auto q{find_biggest_q(data)};
    set_biggest_q(q, data);
    auto list_index{get_list_index(q, data)};

    uint8_t current_max{0};
    uint8_t current_max_index{list_index[1]};

    // we assume to be in the last list which is required to call this method
    for (uint8_t i{list_index[1]}; i < size(data); ++i)
    {
      if (data[7 + i] > current_max)
      {
        current_max = data[7 + i];
        current_max_index = i;
      }
    }

    auto tmp{data[7 + current_max_index]};
    data[7 + current_max_index] = data[7 + size(data) - 1];
    data[7 + size(data) - 1] = tmp;

  }

  // it is assumed that the max has already been set. otherwise u.b.
  static constexpr void evict_max(uint8_t* data)
  {
    const uint8_t biggest_q{get_biggest_q(data)};
    const auto list_indices{get_list_index(biggest_q, data)};
    auto header{get_header(data)};
    uint64_t up_to_deletion
      {header & util::bit_mask_left_rt<uint64_t>::value(static_cast<uint8_t>(list_indices[0] + list_indices[1] - 1))};
    header <<= list_indices[0] + list_indices[1] + 1;
    header >>= list_indices[0] + list_indices[1];
    header |= up_to_deletion;
    set_header(header, data);
  }

  static constexpr uint8_t max(uint8_t* data)
  {
    assert(overflowed(data));

    return data[7 + k - 1];
  }

  static constexpr void mark_overflowed(uint8_t* data)
  {
    uint64_t header_reg{get_header_reg(data)};
    header_reg |= util::bit_mask_position<uint64_t, (k << 1)>::value;
    set_header_reg(header_reg, data);
  }

  static constexpr bool overflowed(uint8_t* data)
  {
    return get_header_reg(data) & (util::bit_mask_position<uint64_t, (k << 1)>::value);
  }

  static constexpr uint8_t get_biggest_q(uint8_t* data)
  {
    uint64_t header_reg{get_header_reg(data)};
    header_reg >>= 8; // move the position of q into the least significant 5 bits
    header_reg &= ~util::bit_mask_left<uint64_t, 58>::value; // 0 everything except for the least significant 5 bits
    return static_cast<uint8_t>(header_reg); // ignore everything except for the first four bits
  }

private:


  static constexpr uint64_t* get_header_ptr(uint8_t* data)
  {
    return reinterpret_cast<uint64_t*>(data);
  }

  static constexpr void set_header(uint64_t new_header, uint8_t* data)
  {
    uint64_t header_cpy{__builtin_bswap64(*get_header_ptr(data))};
    header_cpy &= ~util::bit_mask_left<uint64_t, k << 1>::value;
    new_header |= header_cpy;
    *get_header_ptr(data) = __builtin_bswap64(new_header);
  }

  static constexpr uint64_t get_header(uint8_t* data)
  {

    return (__builtin_bswap64((*reinterpret_cast<uint64_t*>(data))))
      & util::bit_mask_left<uint64_t, (k << 1) - 1>::value;
  }

  static constexpr void set_header_reg(uint64_t new_header_reg, uint8_t* data)
  {
    uint64_t header_cpy{__builtin_bswap64(*get_header_ptr(data))};
    header_cpy &= ~util::bit_mask_left<uint64_t, (k << 1) + (56 - (k << 1))>::value;
    new_header_reg |= header_cpy;
    *get_header_ptr(data) = __builtin_bswap64(new_header_reg);
  }

  // retrieve the memory of the header up to the 56th bit to get the 6 bits up to the beginning of the body
  static constexpr uint64_t get_header_reg(uint8_t* data)
  {
    return (__builtin_bswap64((*reinterpret_cast<uint64_t*>(data))))
      & util::bit_mask_left<uint64_t, (k << 1) + (56 - (k << 1))>::value;
  }

  static constexpr void set_biggest_q(uint8_t new_biggest_q, uint8_t* data)
  {
    // new_biggest q is required to fit in 5 bits otherwise u.b.
    assert(new_biggest_q < k);
    uint64_t header_reg{get_header_reg(data)};
    uint64_t biggest_q_position{~(util::bit_mask_left<uint64_t, 4>::value
      >> ((k << 1) + 1))}; // keep everything except for the biggest_q_position to clear it
    header_reg &= biggest_q_position;
    uint64_t new_biggest_q_64{static_cast<uint64_t>(new_biggest_q)};
    new_biggest_q_64 <<= 8;
    header_reg |= new_biggest_q_64;
    set_header_reg(header_reg, data);
  }

  static constexpr uint8_t find_biggest_q(uint8_t* data)
  {
    uint64_t header{get_header(data)};
    uint8_t pos{0};
    while (header)
    {
      if (!(header & util::bit_mask_position<uint64_t, 0>::value))
      {
        ++pos;
      }
      header <<= 1;
    }

    return pos;
  }

  static constexpr bool insert_into_body(uint8_t index, uint8_t r, uint8_t list_size, uint8_t* data)
  {
    for (uint8_t i{0}; i < list_size; ++i)
    {
      if (data[7 + index + i] == r)
      {
        return false;
      }
    }

    for (uint8_t i{static_cast<uint8_t>(size(data))}; i > index + list_size; --i)
    {
      data[7 + i] = data[7 + i - 1];
    }
    data[7 + index + list_size] = r;
    return true;

  }

  static constexpr void insert_into_header(uint8_t index, uint8_t* data)
  {
    uint64_t header{get_header(data)};

    uint64_t up_to_insertion{(util::bit_mask_left_rt<uint64_t>::value(index) & header)};
    header <<= index;
    header >>= index + 1;
    header |= up_to_insertion;
    header |= util::bit_mask_position_rt<uint64_t>::value(index);

    set_header(header, data);
  }

  static constexpr uint8_t get_list_size(uint8_t list_position, uint8_t* data)
  {
    uint64_t header = get_header(data);
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

  static constexpr std::array<uint8_t, 2> get_list_index(uint8_t q, uint8_t* data)
  {
    uint64_t header{get_header(data)};

    uint8_t remaining{k};
    uint8_t zero_count{0};
    constexpr uint64_t first_bit_mask{util::bit_mask_position<uint64_t, 0>::value}; // <| 0b1000 0000 ... 0000
    while (remaining > 0)
    {
      // we are in the required list
      if (zero_count == q)
      {
        return {static_cast<uint8_t>(zero_count), static_cast<uint8_t>(k - remaining)};
      }
      else if ((header & first_bit_mask))
      {
        --remaining;
      }
      else
      {
        ++zero_count;
      }
      header <<= 1;
    }

    __builtin_unreachable();
  }

};
} // namespace prefix::non_simd

#endif //INCLUDE_PREFIX_NON_SIMD_POCKET_DICTIONARY_HPP
