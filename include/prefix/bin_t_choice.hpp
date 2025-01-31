//
// Created by nils on 1/31/25.
//

#ifndef PREFIX_BIN_CHOICE_HPP
#define PREFIX_BIN_CHOICE_HPP

#include "prefix/bin.hpp"
#include "prefix/adapted/bin.hpp"
#include "prefix/bin_types.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/simd/pocket_dictionary.hpp"

namespace prefix
{
template<bin_types bin_type>
struct bin_t_choice
{
};

template<>
struct bin_t_choice<bin_types::non_simd>
{
  using type = bin<non_simd::pocket_dictionary<25>>;
};

template<>
struct bin_t_choice<bin_types::simd>
{
  using type = bin<simd::pocket_dictionary<25>>;
};

template<>
struct bin_t_choice<bin_types::adapted>
{
  using type = adapted::bin;
};
}// namespace prefix
#endif //PREFIX_BIN_CHOICE_HPP
