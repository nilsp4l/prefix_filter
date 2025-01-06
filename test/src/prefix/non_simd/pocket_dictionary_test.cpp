//
// Created by nils on 11/19/24.
//

#include <random>

#include <gtest/gtest.h>
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/interfaces/i_pocket_dictionary.hpp"
#include "prefix/simd/pocket_dictionary.hpp"

class pocket_dictionary_test : public testing::Test
{
public:
  pocket_dictionary_test()
  {
    this->pd_ = std::make_unique<prefix::simd::pocket_dictionary<25>>();
  }

  std::unique_ptr<prefix::interfaces::i_pocket_dictionary> pd_;
};

TEST_F(pocket_dictionary_test, insert_linear)
{

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{1}; i < 26; ++i)
  {
    pd_->insert(i - 1, i);
    ASSERT_EQ(pd_->size(), i);
    compare_vector.push_back(i);
  }

  for (uint8_t i{1}; i < 26; ++i)
  {
    ASSERT_TRUE(pd_->query(i - 1, i));
    ASSERT_EQ((*pd_)[i - 1], compare_vector[i - 1]);
  }
}

TEST_F(pocket_dictionary_test, insert_all_in_1_list)
{

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{1}; i < 26; ++i)
  {
    pd_->insert(0, i);
    ASSERT_EQ(pd_->size(), i);
    compare_vector.push_back(i);
  }

  for (uint8_t i{1}; i < 26; ++i)
  {
    ASSERT_TRUE(pd_->query(0, i));
    ASSERT_EQ((*pd_)[i - 1], compare_vector[i - 1]);
  }
}

TEST_F(pocket_dictionary_test, double_insert)
{

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{0}; i < 25; ++i)
  {
    pd_->insert(i, i);
    pd_->insert(i, i);
    ASSERT_EQ(pd_->size(), i + 1);
    compare_vector.push_back(i);
  }
  for (uint8_t i{0}; i < 25; ++i)
  {
    ASSERT_TRUE(pd_->query(i, i));
    ASSERT_EQ((*pd_)[i], compare_vector[i]);
  }
}

TEST_F(pocket_dictionary_test, lists_of_size_2)
{

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{0}; i < 12; ++i)
  {
    pd_->insert(i, i);
    pd_->insert(i, i + 1);
    ASSERT_EQ(pd_->size(), 2 * (i + 1));
    compare_vector.push_back(i);
    compare_vector.push_back(i + 1);
  }

  for (uint8_t i{0}; i < 12; ++i)
  {
    ASSERT_TRUE(pd_->query(i, i));
    ASSERT_TRUE(pd_->query(i, i + 1));
  }

  for (uint8_t i{0}; i < 11; ++i)
  {
    for (uint8_t j{i}; j < i + 2; ++j)
    {
      ASSERT_EQ((*pd_)[i], compare_vector[i]);
      ASSERT_EQ((*pd_)[i + 1], compare_vector[i + 1]);
    }

  }
}


TEST_F(pocket_dictionary_test, random_insert_lists_of_5_linear)
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
      pd_->insert(list_index, rand_values[index]);
      if (std::find(list.begin(), list.end(), rand_values[index]) == list.end())
      {
        list.push_back(rand_values[index]);
      }

      ++index;
    }
    ++list_index;
  }


  uint8_t pd_index{0};
  for (list_index = 0; list_index < 5; ++list_index)
  {
    for (uint8_t j{0}; j < compare_array[list_index].size(); ++j)
    {
      ASSERT_TRUE(pd_->query(list_index, compare_array[list_index][j]));
      ASSERT_EQ((*pd_)[pd_index + j], compare_array[list_index][j]);
    }
    pd_index += static_cast<uint8_t>(compare_array[list_index].size());
  }
}


TEST_F(pocket_dictionary_test, random_insert_lists_of_1_shuffle)
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

  std::array<uint8_t, 25> compare_array{};

  for (auto index : random_indices)
  {
    pd_->insert(index, rand_values[index]);
    compare_array[index] = rand_values[index];
  }

  for (auto index : random_indices)
  {
    ASSERT_TRUE(pd_->query(index, rand_values[index]));
    ASSERT_EQ((*pd_)[index], compare_array[index]);
  }
}


TEST_F(pocket_dictionary_test, random_insert_lists_of_5_shuffle)
{
  std::random_device random_device;

  std::mt19937 mers{random_device()};
  std::uniform_int_distribution<uint8_t> distribution(0, UINT8_MAX);

  std::array<uint8_t, 25> rand_values{};

  std::generate(rand_values.begin(), rand_values.end(), [&]()
  {
    return distribution(mers);
  });

  std::array<uint8_t, 5> random_indices{};

  std::iota(random_indices.begin(), random_indices.end(), 0);

  std::shuffle(random_indices.begin(), random_indices.end(), mers);

  std::array<std::vector<uint8_t>, 5> compare_array{};

  std::size_t current_index{0};


  for (auto index : random_indices)
  {
    for (uint8_t i{0}; i < 5; ++i)
    {
      pd_->insert(index, rand_values[current_index]);
      if (std::find(compare_array[index].begin(), compare_array[index].end(), rand_values[current_index])
        == compare_array[index].end())
      {
        compare_array[index].push_back(rand_values[current_index]);
      }

      ++current_index;
    }
  }

  uint8_t pd_index{0};
  for (uint8_t list_index{0}; list_index < 5; ++list_index)
  {
    for (uint8_t j{0}; j < compare_array[list_index].size(); ++j)
    {
      ASSERT_TRUE(pd_->query(list_index, compare_array[list_index][j]));
      ASSERT_EQ((*pd_)[pd_index + j], compare_array[list_index][j]);
    }
    pd_index += static_cast<uint8_t>(compare_array[list_index].size());
  }

}