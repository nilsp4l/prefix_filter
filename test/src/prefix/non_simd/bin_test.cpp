//
// Created by nils on 11/29/24.
//

#include <gtest/gtest.h>
#include <random>
#include <set>
#include "prefix/non_simd/bin.hpp"

class bin_test : public testing::Test
{
public:
  bin_test()
  {

    this->bin_ = std::make_unique<prefix::non_simd::bin<25>>([](uint8_t fp)
    {
      return fp % 25;
    });
  }

  std::unique_ptr<prefix::interfaces::i_bin> bin_;
};

TEST_F(bin_test, insert_linear)
{
  std::vector<uint8_t> compare_vector;
  for (uint8_t i{0}; i < 25; ++i)
  {
    bin_->insert(i);
    ASSERT_EQ(bin_->size(), i + 1);
    compare_vector.push_back(i);
  }

  for (uint8_t i{0}; i < 25; ++i)
  {
    ASSERT_TRUE(bin_->query(i));
    ASSERT_EQ((*bin_)[i], compare_vector[i]);
  }
}

TEST_F(bin_test, random_insert_lookup_size_10)
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

TEST_F(bin_test, random_insert_lookup_size_25)
{
  std::random_device random_device;

  std::mt19937 mers{random_device()};
  std::uniform_int_distribution<uint8_t> distribution(0, UINT8_MAX);

  std::array<uint8_t, 25> rand_values{};
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

