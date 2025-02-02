//
// Created by nils on 01/14/24.
//


#include <gtest/gtest.h>
#include "prefix/spare/bloom/bloom_filter_factory.hpp"

// for a filter we can just test, whether it returns true on keys we are certain on having inserted.
// to validate if it returns false on some amount of negative queries, we need to benchmark it

class bloom_test : public testing::Test
{

public:
  bloom_test() = default;

  static constexpr std::size_t to_insert{1'000'000};

  prefix::spare::bloom::bloom_filter_factory<uint64_t, to_insert>::filter_t filter_{};
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

TEST_F(bloom_test, insert_random_linear)
{

  for (uint64_t i{0}; i < to_insert; ++i)
  {
    filter_.insert(i);
  }

  for (uint64_t i{0}; i < to_insert; ++i)
  {
    ASSERT_TRUE(filter_.query(i));
  }

}

TEST_F(bloom_test, insert_random_)
{
  std::random_device random_device;

  std::mt19937 mers{random_device()};
  std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);

  std::array<uint64_t, to_insert> rand_values{};

  std::generate(rand_values.begin(), rand_values.end(), [&]()
  {
    return distribution(mers);
  });

  for (auto value : rand_values)
  {
    filter_.insert(value);
  }

  for (auto value : rand_values)
  {
    ASSERT_TRUE(filter_.query(value));
  }

}
