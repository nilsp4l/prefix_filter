//
// Created by nils on 11/29/24.
//

#include <gtest/gtest.h>
#include <random>
#include <set>
#include "prefix/bin.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include <variant>
#include <cstdlib>

class bin_test : public testing::TestWithParam<std::variant<prefix::bin<prefix::non_simd::pocket_dictionary<25>>,
                                                            prefix::bin<prefix::simd::pocket_dictionary<25>>>>
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

  uint8_t* data_{nullptr};

};

INSTANTIATE_TEST_SUITE_P(all_bin_tests, bin_test, testing::Values(
  prefix::bin<prefix::non_simd::pocket_dictionary<25>>(),
  prefix::bin<prefix::simd::pocket_dictionary<25>>()
));

TEST_P(bin_test, random_insert_lookup_size_10)
{

  std::visit([&](auto bin_a)
  {
    using bin = decltype(bin_a);
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

  }, GetParam());
}

TEST_P(bin_test, random_insert_lookup_size_25)
{

  std::visit([&](auto bin_a)
  {
    using bin = decltype(bin_a);
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
      bin::insert(rand_val, data_);
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

  }, GetParam());
}

TEST_P(bin_test, linear_insert_max_set_once)
{

  std::visit([&](auto bin_a)
  {
    using bin = decltype(bin_a);
    for (uint8_t i{1}; i < 26; ++i)
    {
      bin::insert(i, data_);
    }

    auto max = bin::insert(0, data_);
    ASSERT_TRUE(max);
    for (uint8_t i{0}; i < 26; ++i)
    {
      if (i == max)
      {
        ASSERT_FALSE(bin::query(i, data_));
      }
      else
      {
        ASSERT_TRUE(bin::query(i, data_));
      }

    }

  }, GetParam());
}

TEST_P(bin_test, insert_10_max_switches)
{

  std::visit([&](auto bin_a)
  {
    using bin = decltype(bin_a);
    for (uint16_t i{1}; i < 26; ++i)
    {
      bin::insert(i * 10, data_);
    }

    for (uint16_t i{0}; i < 10; ++i)
    {
      auto max = bin::insert(i, data_);
      ASSERT_TRUE(max);
      ASSERT_FALSE(bin::query(*max, data_));
    }

  }, GetParam());
}

TEST_P(bin_test, insert_random_field_test)
{

  std::visit([&](auto bin_a)
  {
    using bin = decltype(bin_a);
    std::random_device random_device;

    std::mt19937 mers{random_device()};
    std::uniform_int_distribution<uint16_t> distribution(0, UINT16_MAX);

    for (std::size_t i{0}; i < 100'000; ++i)
    {
      auto current_element{distribution(mers)};
      if (bin::query(current_element, data_))
      {
        continue;
      }
      if (bin::overflowed(data_) || bin::size(data_) == 25)
      {
        auto evicted{bin::insert(current_element, data_)};
        ASSERT_TRUE(evicted);
        ASSERT_EQ(bin::size(data_), 25);
      }
      else
      {
        ASSERT_FALSE(bin::query(current_element, data_));
        ASSERT_FALSE(bin::insert(current_element, data_));
        ASSERT_TRUE(bin::query(current_element, data_));
      }
    }

  }, GetParam());
}


