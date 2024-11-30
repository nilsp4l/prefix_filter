//
// Created by nils on 11/24/24.
//

#ifndef INCLUDE_PREFIX_NON_SIMD_POCKET_DICTIONARY_HPP
#define INCLUDE_PREFIX_NON_SIMD_POCKET_DICTIONARY_HPP

#include "prefix/interfaces/i_pocket_dictionary.hpp"
#include "prefix/util/masks.hpp"

#include <cstdint>

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

  constexpr std::optional<uint8_t> insert(const uint8_t q, const uint8_t r) override
  {

    if (q >= k)
    {
      return std::nullopt;
    }

    // already k elements in data_
    if (size() == k + 1)
    {
      return overflowed_procedure(r);
    }
    auto list_index{get_list_index(q)};
    auto list_size{get_list_size(list_index[0] + list_index[1])};

    // element is inserted because it wasn't already there
    if (insert_into_body(list_index[1], r, list_size))
    {
      insert_into_header(list_index[0] + list_index[1] + list_size);
    }

    return std::nullopt;

  }

  [[nodiscard]] constexpr uint8_t size() const override
  {
    auto header{get_header()};
    return static_cast<uint8_t>(std::popcount(header));
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

    return (__builtin_bswap64((*reinterpret_cast<uint64_t*>(data_)))) & util::bit_mask_left<uint64_t, (k << 1)>::value;
  }

  constexpr uint8_t find_biggest_q()
  {
    uint64_t header{get_header()};
    uint8_t pos{0};
    while (header)
    {
      if (!(header & util::bit_mask_position<uint8_t, 0>::value))
      {
        ++pos;
      }
      header <<= 1;
    }

    return pos;
  }

  constexpr std::optional<uint8_t> max()
  {
    if (overflowed())
    {
      return this->operator[](k - 1);
    }
    return std::nullopt;
  }

  constexpr void mark_overflowed()
  {
  }

  constexpr bool overflowed()
  {
    return (this->operator[](k) & util::bit_mask_position<uint8_t, 0>::value);
  }

  constexpr void set_max(uint8_t new_max)
  {
    this->operator[](k - 1) = new_max;
  }

  constexpr std::optional<uint8_t> overflowed_procedure(uint8_t r)
  {

    if (overflowed() && max() == r)
    {
      return std::nullopt;
    }

    if (overflowed() && max() > r)
    {
      auto tmp_max{max()};
      set_max(r);
      return tmp_max;
    }

    if (overflowed() && max() < r)
    {
      return r;
    }

    assert(!overflowed());


    auto current_max{0};

    if (current_max == r)
    {
      return std::nullopt;
    }

    if (current_max > r)
    {
      set_max(r);
      return current_max;
    }

    if (current_max < r)
    {
      return r;
    }

    __builtin_unreachable();
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

    for (uint8_t i{static_cast<uint8_t>(size() + 1)}; i > index + list_size; --i)
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
    constexpr uint64_t first_bit_mask{util::bit_mask_position<uint64_t, 0>::value}; // <| 0b1000 0000
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
