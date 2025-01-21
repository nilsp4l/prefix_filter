//
// Created by nils on 11/19/24.
//

#include <random>
#include <cstdlib>
#include <gtest/gtest.h>
#include "prefix/adapted/bin.hpp"

using bin = prefix::adapted::bin;

class adapted_bin_test : public testing::Test
{
public:

  void SetUp() override
  {
    data_ = reinterpret_cast<uint8_t*>(std::aligned_alloc(32, 32));
    for (std::size_t i{0}; i < 32; ++i)
    {
      data_[i] = 0;
    }
  }

  void TearDown() override
  {
    std::free(data_);
  }

  [[nodiscard]] uint8_t find_maximum_linear() const
  {
    uint8_t current_max{0};
    for (uint8_t i{1}; i < 32; ++i)
    {
      if (data_[i] > current_max)
      {
        current_max = data_[i];
      }
    }
    return current_max;
  }

  uint8_t* data_{nullptr};
};

TEST_F(adapted_bin_test, random_insert_lookup_size_10)
{
  std::random_device random_device;

  std::mt19937 mers{random_device()};
  std::uniform_int_distribution<uint8_t> distribution(0, UINT8_MAX);

  std::array<uint8_t, 10> rand_values{};
  std::set<uint8_t> rand_set{};

  std::generate(rand_values.begin(), rand_values.end(), [&]()
  {
    return distribution(mers);
  });

  for (auto rand_val : rand_values)
  {
    rand_set.insert(rand_val);
    bin::insert(rand_val, data_);
    ASSERT_EQ(rand_set.size(), bin::size(data_));
  }

  for (uint8_t fp{0}; fp < UINT8_MAX; ++fp)
  {
    if (rand_set.contains(fp))
    {
      ASSERT_TRUE(bin::query(fp, data_));
    }
    else
    {
      ASSERT_FALSE(bin::query(fp, data_));
    }
  }

}

TEST_F(adapted_bin_test, random_insert_lookup_size_31)
{
  std::random_device random_device;

  std::mt19937 mers{random_device()};
  std::uniform_int_distribution<uint8_t> distribution(0, UINT8_MAX);

  std::array<uint8_t, 31> rand_values{};
  std::set<uint8_t> rand_set{};

  std::generate(rand_values.begin(), rand_values.end(), [&]()
  {
    return distribution(mers);
  });

  for (auto rand_val : rand_values)
  {
    rand_set.insert(rand_val);
    bin::insert(rand_val, data_);
    ASSERT_EQ(rand_set.size(), bin::size(data_));
  }

  for (uint8_t fp{0}; fp < UINT8_MAX; ++fp)
  {
    if (rand_set.contains(fp))
    {
      ASSERT_TRUE(bin::query(fp, data_));
    }
    else
    {
      ASSERT_FALSE(bin::query(fp, data_));
    }
  }
}

TEST_F(adapted_bin_test, linear_insert_max_set_once)
{
  for (uint8_t i{1}; i < 32; ++i)
  {
    bin::insert(i, data_);
  }
  bin::insert(0, data_);

  for (uint8_t i{0}; i < 31; ++i)
  {
    ASSERT_TRUE(bin::query(i, data_));
  }


  ASSERT_FALSE(bin::query(31, data_));
  ASSERT_TRUE(bin::query(30, data_));
}

TEST_F(adapted_bin_test, insert_max_set_multiple_times)
{
  for (uint8_t i{UINT8_MAX - 30}; i < UINT8_MAX; ++i)
  {

    ASSERT_FALSE(bin::insert(i, data_));

  }
  ASSERT_FALSE(bin::insert(UINT8_MAX, data_));

  for (uint8_t i{UINT8_MAX - 30}; i < UINT8_MAX; ++i)
  {
    ASSERT_TRUE(bin::query(i, data_));
  }

  ASSERT_TRUE(bin::query(UINT8_MAX, data_));

  for (uint8_t i{0}; i < UINT8_MAX - 30; ++i)
  {
    std::optional<uint8_t> return_val{};
    uint8_t max{find_maximum_linear()};

    ASSERT_TRUE((return_val = bin::insert(i, data_)));
    ASSERT_GE(*return_val, max);
    ASSERT_EQ(bin::size(data_), 31);
    if (i <= max)
    {
      ASSERT_TRUE(bin::query(i, data_));
    }
    else
    {
      ASSERT_FALSE(bin::query(i, data_));
    }

    ASSERT_FALSE(bin::query(*return_val, data_));
  }
}

