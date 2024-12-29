//
// Created by nils on 11/19/24.
//

#include <random>

#include <gtest/gtest.h>
#include "prefix/adapted/pocket_dictionary.hpp"


class adapted_pocket_dictionary_test : public testing::Test
{
public:
  adapted_pocket_dictionary_test()
  {
    this->pd_ = std::make_unique<prefix::adapted::pocket_dictionary>();
  }

  std::unique_ptr<prefix::adapted::pocket_dictionary> pd_;
};

TEST_F(adapted_pocket_dictionary_test, insert_linear)
{

  for (uint8_t i{1}; i < 26; ++i)
  {
    pd_->insert(i);
    ASSERT_EQ(pd_->size(), i);
  }

  for (uint8_t i{1}; i < 26; ++i)
  {
    ASSERT_TRUE(pd_->query(i));
  }
}

TEST_F(adapted_pocket_dictionary_test, insert_all_in_1_list)
{

  for (uint8_t i{1}; i < 26; ++i)
  {
    pd_->insert(i);
    ASSERT_EQ(pd_->size(), i);
  }

  for (uint8_t i{1}; i < 26; ++i)
  {
    ASSERT_TRUE(pd_->query(i));
  }
}

TEST_F(adapted_pocket_dictionary_test, double_insert)
{

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{0}; i < 25; ++i)
  {
    pd_->insert(i);
    pd_->insert(i);
    ASSERT_EQ(pd_->size(), i + 1);
  }
  for (uint8_t i{0}; i < 25; ++i)
  {
    ASSERT_TRUE(pd_->query(i));
  }
}


TEST_F(adapted_pocket_dictionary_test, random_insert_lists_of_5_linear)
{
  std::random_device random_device;

  std::mt19937 mers{random_device()};
  std::uniform_int_distribution<uint8_t> distribution(0, UINT8_MAX);

  std::array<uint8_t, 25> rand_values{};

  std::generate(rand_values.begin(), rand_values.end(), [&]()
  {
    return distribution(mers);
  });

  std::array<std::vector<uint8_t>, 5> compare_array{};

  std::size_t index{0};
  uint8_t list_index{0};
  for (auto& list : compare_array)
  {
    for (uint8_t i{0}; i < 5; ++i)
    {
      pd_->insert(rand_values[index]);
      if (std::find(list.begin(), list.end(), rand_values[index]) == list.end())
      {
        list.push_back(rand_values[index]);
      }

      ++index;
    }
  }


  uint8_t pd_index{0};
  for (list_index = 0; list_index < 5; ++list_index)
  {
    for (uint8_t j{0}; j < compare_array[list_index].size(); ++j)
    {
      ASSERT_TRUE(pd_->query(compare_array[list_index][j]));
    }
    pd_index += compare_array[list_index].size();
  }
}

TEST_F(adapted_pocket_dictionary_test, random_insert_lists_of_1_shuffle)
{
  std::random_device random_device;

  std::mt19937 mers{random_device()};
  std::uniform_int_distribution<uint8_t> distribution(0, UINT8_MAX);

  std::array<uint8_t, 25> rand_values{};

  std::generate(rand_values.begin(), rand_values.end(), [&]()
  {
    return distribution(mers);
  });

  std::array<uint8_t, 25> random_indices{};

  std::iota(random_indices.begin(), random_indices.end(), 0);

  std::shuffle(random_indices.begin(), random_indices.end(), mers);


  for (auto index : random_indices)
  {
    pd_->insert(rand_values[index]);
  }

  for (auto index : random_indices)
  {
    ASSERT_TRUE(pd_->query(rand_values[index]));
  }
}

