//
// Created by nils on 11/19/24.
//

#include <random>

#include <gtest/gtest.h>
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/interfaces/i_pocket_dictionary.hpp"
#include "prefix/simd/pocket_dictionary.hpp"

// we need to use shared_ptrs here, because GetParam() uses the copy constructor
// but we do not need to take care of performance inside the tests, so that's fine
class pocket_dictionary_test : public testing::TestWithParam<std::shared_ptr<prefix::interfaces::i_pocket_dictionary>>
{
public:
  pocket_dictionary_test()
  {
  }

};

INSTANTIATE_TEST_SUITE_P(
  all_pocket_dictionary_tests,
  pocket_dictionary_test,
  testing::Values(
    std::make_shared<prefix::non_simd::pocket_dictionary<25>>(),
    std::make_shared<prefix::simd::pocket_dictionary<25>>()
  ));


TEST_P(pocket_dictionary_test, insert_linear)
{

  auto pd{GetParam()};

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{1}; i < 26; ++i)
  {
    pd->insert(i - 1, i);
    ASSERT_EQ(pd->size(), i);
    compare_vector.push_back(i);
  }

  for (uint8_t i{1}; i < 26; ++i)
  {
    ASSERT_TRUE(pd->query(i - 1, i));
    ASSERT_EQ((*pd)[i - 1], compare_vector[i - 1]);
  }
}

TEST_P(pocket_dictionary_test, insert_all_in_1_list)
{

  auto pd{GetParam()};

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{1}; i < 26; ++i)
  {
    pd->insert(0, i);
    ASSERT_EQ(pd->size(), i);
    compare_vector.push_back(i);
  }

  for (uint8_t i{1}; i < 26; ++i)
  {
    ASSERT_TRUE(pd->query(0, i));
    ASSERT_EQ((*pd)[i - 1], compare_vector[i - 1]);
  }
}

TEST_P(pocket_dictionary_test, double_insert)
{

  auto pd{GetParam()};

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{0}; i < 25; ++i)
  {
    pd->insert(i, i);
    pd->insert(i, i);
    ASSERT_EQ(pd->size(), i + 1);
    compare_vector.push_back(i);
  }
  for (uint8_t i{0}; i < 25; ++i)
  {
    ASSERT_TRUE(pd->query(i, i));
    ASSERT_EQ((*pd)[i], compare_vector[i]);
  }
}

TEST_P(pocket_dictionary_test, lists_of_size_2)
{

  auto pd{GetParam()};

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{0}; i < 12; ++i)
  {
    pd->insert(i, i);
    pd->insert(i, i + 1);
    ASSERT_EQ(pd->size(), 2 * (i + 1));
    compare_vector.push_back(i);
    compare_vector.push_back(i + 1);
  }

  for (uint8_t i{0}; i < 12; ++i)
  {
    ASSERT_TRUE(pd->query(i, i));
    ASSERT_TRUE(pd->query(i, i + 1));
  }

  for (uint8_t i{0}; i < 11; ++i)
  {
    for (uint8_t j{i}; j < i + 2; ++j)
    {
      ASSERT_EQ((*pd)[i], compare_vector[i]);
      ASSERT_EQ((*pd)[i + 1], compare_vector[i + 1]);
    }

  }
}


TEST_P(pocket_dictionary_test, random_insert_lists_of_5_linear)
{

  auto pd{GetParam()};

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
      pd->insert(list_index, rand_values[index]);
      if (std::find(list.begin(), list.end(), rand_values[index]) == list.end())
      {
        list.push_back(rand_values[index]);
      }

      ++index;
    }
    ++list_index;
  }


  uint8_t pdindex{0};
  for (list_index = 0; list_index < 5; ++list_index)
  {
    for (uint8_t j{0}; j < compare_array[list_index].size(); ++j)
    {
      ASSERT_TRUE(pd->query(list_index, compare_array[list_index][j]));
      ASSERT_EQ((*pd)[pdindex + j], compare_array[list_index][j]);
    }
    pdindex += static_cast<uint8_t>(compare_array[list_index].size());
  }
}


TEST_P(pocket_dictionary_test, random_insert_lists_of_1_shuffle)
{

  auto pd{GetParam()};

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
    pd->insert(index, rand_values[index]);
    compare_array[index] = rand_values[index];
  }

  for (auto index : random_indices)
  {
    ASSERT_TRUE(pd->query(index, rand_values[index]));
    ASSERT_EQ((*pd)[index], compare_array[index]);
  }
}


TEST_P(pocket_dictionary_test, random_insert_lists_of_5_shuffle)
{

  auto pd{GetParam()};

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
      pd->insert(index, rand_values[current_index]);
      if (std::find(compare_array[index].begin(), compare_array[index].end(), rand_values[current_index])
        == compare_array[index].end())
      {
        compare_array[index].push_back(rand_values[current_index]);
      }

      ++current_index;
    }
  }

  uint8_t pdindex{0};
  for (uint8_t list_index{0}; list_index < 5; ++list_index)
  {
    for (uint8_t j{0}; j < compare_array[list_index].size(); ++j)
    {
      ASSERT_TRUE(pd->query(list_index, compare_array[list_index][j]));
      ASSERT_EQ((*pd)[pdindex + j], compare_array[list_index][j]);
    }
    pdindex += static_cast<uint8_t>(compare_array[list_index].size());
  }

}