//
// Created by nils on 11/24/24.
//

#ifndef INCLUDE_PREFIX_NON_SIMD_POCKET_DICTIONARY_HPP
#define INCLUDE_PREFIX_NON_SIMD_POCKET_DICTIONARY_HPP

#include "prefix/interfaces/i_pocket_dictionary.hpp"
#include "util/masks.hpp"

#include <cstdint>
#include <assert.h>

namespace prefix::non_simd
{
/// this is the naive implementation used mainly for comparison and base of the optimized ones, as
/// it is much easier to understand.
/// !Not optimized at all!
template<uint8_t k>
class pocket_dictionary : public interfaces::i_pocket_dictionary
{
public:
  pocket_dictionary()
  {
    static_assert(k <= 25 && k > 0);
    // initialized with 0, which is important for the header to be initialized with no elements in it
    data_ = new uint8_t[32]();
  }

  ~pocket_dictionary() override
  {
    delete[] data_;
  }

  uint8_t& operator[](uint8_t index) const override
  {
    return data_[7 + index];
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

    // q is greater than the amount of lists there is or already k elements in data_
    if (q >= k || size() == k + 1)
    {
      return;
    }

    auto list_index{get_list_index(q)};
    auto list_size{get_list_size(list_index[0] + list_index[1])};

    // element is inserted because it wasn't already there
    if (insert_into_body(list_index[1], r, list_size))
    {
      insert_into_header(list_index[0] + list_index[1] + list_size);
    }

  }

  [[nodiscard]] constexpr uint8_t size() const override
  {
    auto header{get_header()};
    return static_cast<uint8_t>(std::popcount(header));
  }

  constexpr void max_move_procedure() override
  {
    auto q{find_biggest_q()};
    set_biggest_q(q);
    auto list_index{get_list_index(q)};

    uint8_t current_max{0};
    uint8_t current_max_index{list_index[1]};

    // we assume to be in the last list which is required to call this method
    for (uint8_t i{list_index[1]}; i < size(); ++i)
    {
      if (this->operator[](i) > current_max)
      {
        current_max = this->operator[](i);
        current_max_index = i;
      }
    }

    auto tmp{this->operator[](current_max_index)};
    this->operator[](current_max_index) = this->operator[](size() - 1);
    this->operator[](size() - 1) = tmp;

  }

  // it is assumed that the max has already been set. otherwise u.b.
  constexpr void evict_max() override
  {
    const uint8_t biggest_q{get_biggest_q()};
    const auto list_indices{get_list_index(biggest_q)};
    auto header{get_header()};
    uint64_t up_to_deletion{header & util::bit_mask_left_rt<uint64_t>::value(list_indices[0] + list_indices[1] - 1)};
    header <<= list_indices[0] + list_indices[1] + 1;
    header >>= list_indices[0] + list_indices[1];
    header |= up_to_deletion;
    set_header(header);
  }

  [[nodiscard]] constexpr uint8_t max() const override
  {
    assert(overflowed());

    return this->operator[](k - 1);
  }

  constexpr void mark_overflowed() override
  {
    uint64_t header_reg{get_header_reg()};
    header_reg |= util::bit_mask_position<uint64_t, (k << 1)>::value;
    set_header_reg(header_reg);
  }

  [[nodiscard]] constexpr bool overflowed() const override
  {
    return get_header_reg() & (util::bit_mask_position<uint64_t, (k << 1)>::value);
  }

private:


  constexpr uint64_t* get_header_ptr()
  {
    return reinterpret_cast<uint64_t*>(data_);
  }

  constexpr void set_header(uint64_t new_header)
  {
    uint64_t header_cpy{__builtin_bswap64(*get_header_ptr())};
    header_cpy &= ~util::bit_mask_left<uint64_t, k << 1>::value;
    new_header |= header_cpy;
    *get_header_ptr() = __builtin_bswap64(new_header);
  }

  [[nodiscard]] constexpr uint64_t get_header() const
  {

    return (__builtin_bswap64((*reinterpret_cast<uint64_t*>(data_))))
      & util::bit_mask_left<uint64_t, (k << 1) - 1>::value;
  }

  constexpr void set_header_reg(uint64_t new_header_reg)
  {
    uint64_t header_cpy{__builtin_bswap64(*get_header_ptr())};
    header_cpy &= ~util::bit_mask_left<uint64_t, (k << 1) + (56 - (k << 1))>::value;
    new_header_reg |= header_cpy;
    *get_header_ptr() = __builtin_bswap64(new_header_reg);
  }

  // retrieve the memory of the header up to the 56th bit to get the 6 bits up to the beginning of the body
  [[nodiscard]] constexpr uint64_t get_header_reg() const
  {
    return (__builtin_bswap64((*reinterpret_cast<uint64_t*>(data_))))
      & util::bit_mask_left<uint64_t, (k << 1) + (56 - (k << 1))>::value;
  }

  constexpr uint8_t get_biggest_q()
  {
    uint64_t header_reg{get_header_reg()};
    header_reg >>= 8; // move the position of q into the least significant 5 bits
    header_reg &= ~util::bit_mask_left<uint64_t, 58>::value; // 0 everything except for the least significant 5 bits
    return static_cast<uint8_t>(header_reg); // ignore everything except for the first four bits
  }

  constexpr void set_biggest_q(uint8_t new_biggest_q)
  {
    // new_biggest q is required to fit in 5 bits otherwise u.b.
    assert(new_biggest_q < k);
    uint64_t header_reg{get_header_reg()};
    uint64_t biggest_q_position{~(util::bit_mask_left<uint64_t, 4>::value
      >> ((k << 1) + 1))}; // keep everything except for the biggest_q_position to clear it
    header_reg &= biggest_q_position;
    uint64_t new_biggest_q_64{static_cast<uint64_t>(new_biggest_q)};
    new_biggest_q_64 <<= 8;
    header_reg |= new_biggest_q_64;
    set_header_reg(header_reg);
  }

  [[nodiscard]] constexpr uint8_t find_biggest_q() const
  {
    uint64_t header{get_header()};
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

  constexpr bool insert_into_body(uint8_t index, uint8_t r, uint8_t list_size)
  {
    for (uint8_t i{0}; i < list_size; ++i)
    {
      if (this->operator[](index + i) == r)
      {
        return false;
      }
    }

    for (uint8_t i{static_cast<uint8_t>(size())}; i > index + list_size; --i)
    {
      this->operator[](i) = this->operator[](i - 1);
    }
    this->operator[](index + list_size) = r;
    return true;

  }

  constexpr void insert_into_header(uint8_t index)
  {
    uint64_t header{get_header()};

    uint64_t up_to_insertion{(util::bit_mask_left_rt<uint64_t>::value(index) & header)};
    header <<= index;
    header >>= index + 1;
    header |= up_to_insertion;
    header |= util::bit_mask_position_rt<uint64_t>::value(index);

    set_header(header);
  }

  [[nodiscard]] constexpr uint8_t get_list_size(uint8_t list_position) const
  {
    uint64_t header = get_header();
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
    uint64_t header{get_header()};

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

  uint8_t* data_;
};
} // namespace prefix::non_simd

#endif //INCLUDE_PREFIX_NON_SIMD_POCKET_DICTIONARY_HPP
