//
// Created by nils on 1/18/25.
//

#include <gtest/gtest.h>
#include "prefix/prefix_filter.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "prefix/adapted/bin.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/bin.hpp"

class prefix_filter_test : public testing::Test
{
public:
  prefix_filter_test() = default;

  prefix::prefix_filter<uint64_t,
                        prefix::adapted::bin,
                        256 /*prefix::bin<25, prefix::simd::pocket_dictionary<25>>, 256*/> filter_{};
};


// we want to only have false positives not false negatives
TEST_F(prefix_filter_test, insert_linear)
{
  for (uint64_t i{0}; i < 252'000'000; ++i)
  {
    filter_.insert(i);
  }

  for (uint64_t i{0}; i < 252'000'000; ++i)
  {
    ASSERT_TRUE(filter_.query(i));
  }
}