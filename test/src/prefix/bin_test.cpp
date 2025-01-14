//
// Created by nils on 11/29/24.
//

#include <gtest/gtest.h>
#include <random>
#include <set>
#include "prefix/bin.hpp"

#include <iostream>

class bin_test : public testing::TestWithParam<std::shared_ptr<prefix::interfaces::i_bin>>
{
public:
  bin_test()
  {
  }

};


INSTANTIATE_TEST_CASE_P(all_bin_tests, bin_test, testing::Values(
  std::make_shared<prefix::bin<25, prefix::non_simd::pocket_dictionary<25>>>(),
  std::make_shared<prefix::bin<25, prefix::simd::pocket_dictionary<25>>>()
));


TEST_P(bin_test, random_insert_lookup_size_10)
{

  auto bin{GetParam()};

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
    bin->insert(rand_val);
    ASSERT_EQ(rand_set.size(), bin->size());
  }

  for (uint8_t fp{0}; fp < UINT8_MAX; ++fp)
  {
    if (rand_set.contains(fp))
    {
      ASSERT_TRUE(bin->query(fp));
    }
    else
    {
      ASSERT_FALSE(bin->query(fp));
    }
  }

}

TEST_P(bin_test, random_insert_lookup_size_25)
{

  auto bin{GetParam()};

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
    bin->insert(rand_val);
    ASSERT_EQ(rand_set.size(), bin->size());
  }

  for (uint8_t fp{0}; fp < UINT8_MAX; ++fp)
  {
    if (rand_set.contains(fp))
    {
      ASSERT_TRUE(bin->query(fp));
    }
    else
    {
      ASSERT_FALSE(bin->query(fp));
    }
  }
}

TEST_P(bin_test, linear_insert_max_set_once)
{

  auto bin{GetParam()};

  for (uint8_t i{1}; i < 26; ++i)
  {
    bin->insert(i);
  }

  bin->insert(0);

  for (uint8_t i{1}; i < 25; ++i)
  {
    ASSERT_TRUE(bin->query(i));
  }


  ASSERT_FALSE(bin->query(25));
  ASSERT_TRUE(bin->query(24));
}


TEST_P(bin_test, insert_10_max_switches)
{

  auto bin{GetParam()};

  for (uint8_t i{1}; i < 26; ++i)
  {
    bin->insert(i * 10);
  }


  uint8_t current_max{250};
  ASSERT_EQ((*bin)[24], current_max);

  for (uint8_t i{0}; i < 10; ++i)
  {
    bin->insert(i);

    current_max -= 10;
    ASSERT_EQ((*bin)[24], current_max);
  }

  for (uint8_t i{0}; i < 10; ++i)
  {
    ASSERT_TRUE(bin->query(i));
  }

}

