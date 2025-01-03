//
// Created by nils on 11/19/24.
//

#include <random>

#include <gtest/gtest.h>
#include "prefix/adapted/bin.hpp"


class adapted_bin_test : public testing::Test
{
public:
  adapted_bin_test()
  {
    this->bin_ = std::make_unique<prefix::adapted::bin>();
  }

  std::unique_ptr<prefix::interfaces::i_bin> bin_;
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
    bin_->insert(rand_val);
    ASSERT_EQ(rand_set.size(), bin_->size());
  }

  for (uint8_t fp{0}; fp < UINT8_MAX; ++fp)
  {
    if (rand_set.contains(fp))
    {
      ASSERT_TRUE(bin_->query(fp));
    }
    else
    {
      ASSERT_FALSE(bin_->query(fp));
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
    bin_->insert(rand_val);
    ASSERT_EQ(rand_set.size(), bin_->size());
  }

  for (uint8_t fp{0}; fp < UINT8_MAX; ++fp)
  {
    if (rand_set.contains(fp))
    {
      ASSERT_TRUE(bin_->query(fp));
    }
    else
    {
      ASSERT_FALSE(bin_->query(fp));
    }
  }
}

TEST_F(adapted_bin_test, linear_insert_max_set_once)
{
  for (uint8_t i{1}; i < 32; ++i)
  {
    bin_->insert(i);
  }
  bin_->insert(0);

  for (uint8_t i{0}; i < 31; ++i)
  {
    ASSERT_TRUE(bin_->query(i));
  }


  ASSERT_FALSE(bin_->query(31));
  ASSERT_TRUE(bin_->query(30));
}

