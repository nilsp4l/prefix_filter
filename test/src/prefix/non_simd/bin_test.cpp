//
// Created by nils on 11/19/24.
//

#include <gtest/gtest.h>
#include "prefix/non_simd/bin.hpp"
#include "prefix/interfaces/i_bin.hpp"

class bin_test : public testing::Test {
 public:
  bin_test ()
  {
    this->bin_ = std::make_unique<prefix::non_simd::bin<25>> ();
  }

  std::unique_ptr<prefix::interfaces::i_bin> bin_;
};

TEST_F(bin_test, insert_linear)
{

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{0}; i < 25; ++i)
    {
      bin_->insert (i, i);
      ASSERT_EQ(bin_->size (), i + 1);
      compare_vector.push_back (i);
    }
  for (uint8_t i{0}; i < 25; ++i)
    {
      ASSERT_TRUE(bin_->query (i, i));
      ASSERT_EQ((*bin_)[i], compare_vector[i]);
    }
}

TEST_F(bin_test, double_insert)
{

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{0}; i < 25; ++i)
    {
      bin_->insert (i, i);
      bin_->insert (i, i);
      ASSERT_EQ(bin_->size (), i + 1);
      compare_vector.push_back (i);
    }
  for (uint8_t i{0}; i < 25; ++i)
    {
      ASSERT_TRUE(bin_->query (i, i));
      ASSERT_EQ((*bin_)[i], compare_vector[i]);
    }
}

TEST_F(bin_test, lists_of_size_2)
{

  std::vector<uint8_t> compare_vector;
  for (uint8_t i{0}; i < 12; ++i)
    {
      bin_->insert (i, i);
      bin_->insert (i, i + 1);
      ASSERT_EQ(bin_->size (), 2 * (i + 1));
      compare_vector.push_back (i);
      compare_vector.push_back (i + 1);
    }

  for (uint8_t i{0}; i < 12; ++i)
    {
      ASSERT_TRUE(bin_->query (i, i));
      ASSERT_TRUE(bin_->query (i, i + 1));
      ASSERT_EQ((*bin_)[i], compare_vector[i]);
      ASSERT_EQ((*bin_)[i + 1], compare_vector[i + 1]);

    }
}