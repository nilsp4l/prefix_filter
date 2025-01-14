//
// Created by nils on 01/14/24.
//


#include <gtest/gtest.h>
#include <bloom/bloom_filter.hpp>

class bloom_test : public testing::Test
{
public:
  bloom_test() = default;

  bloom_filter<uint16_t> filter_;
};


TEST_F(bloom_test, insert_10_elements)
{
  std::set<uint16_t> keys{5, 212, 1337, 420, 42, 12, 21230, 900, 42042, 65000};
  for (uint16_t key : keys)
  {
    filter_.insert(key);
  }

  for (uint16_t key : keys)
  {
    ASSERT_TRUE(filter_.query(key));
  }
}
