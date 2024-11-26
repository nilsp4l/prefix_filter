//
// Created by nils on 11/19/24.
//

#include <gtest/gtest.h>
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/interfaces/i_pocket_dictionary.hpp"

class pocket_dictionary_test : public testing::Test
{
public:
  pocket_dictionary_test()
  {
    this->pd_ = std::make_unique<prefix::non_simd::pocket_dictionary<25>>();
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
    ASSERT_EQ((*pd_)[i], compare_vector[i]);
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
    ASSERT_EQ((*pd_)[i], compare_vector[i]);
    ASSERT_EQ((*pd_)[i + 1], compare_vector[i + 1]);

  }
}