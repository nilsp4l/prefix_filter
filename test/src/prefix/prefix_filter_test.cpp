//
// Created by nils on 1/18/25.
//

#include <gtest/gtest.h>
#include "prefix/prefix_filter.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "prefix/adapted/bin.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/bin.hpp"
#include "prefix/prefix_filter_factory.hpp"


constexpr uint64_t to_insert{5000000};

class prefix_filter_test : public testing::Test
{
public:
  prefix_filter_test() = default;

  prefix::prefix_filter_factory<uint64_t, prefix::bin_types::simd, prefix::spare::types::bloom, to_insert>::filter_t
    filter_{};
};


// we want to only have false positives not false negatives
TEST_F(prefix_filter_test, insert_linear)
{
  for (uint64_t i{0}; i < to_insert; ++i)
  {
    if (i == 41936)
    {
      int k = 0;
    }
    filter_.insert(i);
  }

  for (uint64_t i{0}; i < to_insert; ++i)
  {
    if (!filter_.query(i))
    {
      std::cout << std::to_string(i) << std::endl;
      filter_.query(i);
    }
    ASSERT_TRUE(filter_.query(i));
  }
}
