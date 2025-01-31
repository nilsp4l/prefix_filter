//
// Created by nils on 1/18/25.
//

#include <gtest/gtest.h>
#include <array>
#include <variant>
#include "prefix/prefix_filter.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include "prefix/adapted/bin.hpp"
#include "prefix/non_simd/pocket_dictionary.hpp"
#include "prefix/bin.hpp"
#include "prefix/prefix_filter_factory.hpp"


constexpr uint64_t to_insert{1'000'000};

class prefix_filter_test : public testing::TestWithParam<std::variant<std::shared_ptr<prefix::prefix_filter_factory<
  uint64_t,
  prefix::bin_types::non_simd,
  prefix::spare::types::bloom,
  to_insert>::filter_t>,
                                                                      std::shared_ptr<prefix::prefix_filter_factory<
                                                                        uint64_t,
                                                                        prefix::bin_types::simd,
                                                                        prefix::spare::types::bloom,
                                                                        to_insert>::filter_t>,
                                                                      std::shared_ptr<prefix::prefix_filter_factory<
                                                                        uint64_t,
                                                                        prefix::bin_types::adapted,
                                                                        prefix::spare::types::bloom,
                                                                        to_insert>::filter_t>,
                                                                      std::shared_ptr<prefix::prefix_filter_factory<
                                                                        uint64_t,
                                                                        prefix::bin_types::non_simd,
                                                                        prefix::spare::types::prefix_adapted,
                                                                        to_insert>::filter_t>,
                                                                      std::shared_ptr<prefix::prefix_filter_factory<
                                                                        uint64_t,
                                                                        prefix::bin_types::simd,
                                                                        prefix::spare::types::prefix_adapted,
                                                                        to_insert>::filter_t>,
                                                                      std::shared_ptr<prefix::prefix_filter_factory<
                                                                        uint64_t,
                                                                        prefix::bin_types::adapted,
                                                                        prefix::spare::types::prefix_adapted,
                                                                        to_insert>::filter_t>>>
{
public:
  prefix_filter_test() = default;
};

INSTANTIATE_TEST_SUITE_P(
  all_prefix_filter_tests,
  prefix_filter_test,
  testing::Values(std::make_shared<prefix::prefix_filter_factory<uint64_t,
                                                                 prefix::bin_types::non_simd,
                                                                 prefix::spare::types::bloom,
                                                                 to_insert>::filter_t>(),
    std::make_shared<prefix::prefix_filter_factory<uint64_t,
                                                   prefix::bin_types::simd,
                                                   prefix::spare::types::bloom,
                                                   to_insert>::filter_t>(),
    std::make_shared<prefix::prefix_filter_factory<uint64_t,
                                                   prefix::bin_types::adapted,
                                                   prefix::spare::types::bloom,
                                                   to_insert>::filter_t>(),
    std::make_shared<prefix::prefix_filter_factory<uint64_t,
                                                   prefix::bin_types::non_simd,
                                                   prefix::spare::types::prefix_adapted,
                                                   to_insert>::filter_t>(),
    std::make_shared<prefix::prefix_filter_factory<uint64_t,
                                                   prefix::bin_types::simd,
                                                   prefix::spare::types::prefix_adapted,
                                                   to_insert>::filter_t>(),
    std::make_shared<prefix::prefix_filter_factory<
      uint64_t,
      prefix::bin_types::adapted,
      prefix::spare::types::prefix_adapted,
      to_insert>::filter_t>()
  ));


// we want to only have false positives not false negatives
TEST_P(prefix_filter_test, insert_linear)
{
  auto filter_param{GetParam()};
  std::visit([](auto filter)
  {
    for (uint64_t i{0}; i < to_insert; ++i)
    {

      filter->insert(i);
    }

    for (uint64_t i{0}; i < to_insert; ++i)
    {
      if (!filter->query(i))
      {
        filter->query(i);
      }
      ASSERT_TRUE(filter->query(i));
    }
  }, filter_param);

}

TEST_P(prefix_filter_test, insert_random)
{
  auto filter_param{GetParam()};
  std::visit([](auto filter)
  {
    std::random_device random_device;

    std::mt19937 mers{random_device()};
    std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);

    std::array<uint64_t, to_insert> rand_values{};

    std::generate(rand_values.begin(), rand_values.end(), [&]()
    {
      return distribution(mers);
    });

    for (auto key : rand_values)
    {
      filter->insert(key);
    }

    for (auto key : rand_values)
    {
      ASSERT_TRUE(filter->query(key));
    }
  }, filter_param);

}
