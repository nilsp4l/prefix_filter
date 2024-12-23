//
// Created by nils on 12/12/24.
//

#ifndef PREFIX_SIMD_POCKET_DICTIONARY_HPP
#define PREFIX_SIMD_POCKET_DICTIONARY_HPP

#include <cstdint>
#include <immintrin.h>

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
      return true;
    }

    constexpr void insert(uint8_t q, uint8_t r) override
    {

      auto current_state{_mm256_loadu_si256(reinterpret_cast<__m256i *>(data_))};
      auto header{get_header()};
      uint8_t q_cpy{q};
      uint8_t body_position{0};
      uint8_t header_position{0};
      while(q_cpy)
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

      while (header & util::bit_mask_position<uint64_t, 0>::value)
      {
        ++body_position;
        ++header_position;
      }

      insert_into_header(header_position);
      
    }

    [[nodiscard]] constexpr uint8_t size() const override
    {
      return 0;
    }

    constexpr void evict_max() override
    {
    }

    constexpr void max_move_procedure() override
    {
    }

    [[nodiscard]] constexpr uint8_t max() const override
    {
      return 0;
    }

    constexpr void mark_overflowed() override
    {
    }

    [[nodiscard]] constexpr bool overflowed() const override
    {
      return true;
    }

  private:
    constexpr uint64_t *get_header_ptr()
    {
      return reinterpret_cast<uint64_t *>(data_);
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

      return (__builtin_bswap64((*reinterpret_cast<uint64_t *>(data_)))) & util::bit_mask_left<uint64_t, (k << 1) - 1>::value;
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

    uint8_t *data_;
  };

} // namespace prefix::simd

#endif // PREFIX_SIMD_POCKET_DICTIONARY_HPP
