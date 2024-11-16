#include <memory>

#include "prefix/non_simd/bin.hpp"
#include "prefix/interfaces/i_bin.hpp"

int main()
{
  std::unique_ptr<prefix::interfaces::i_bin> bin{std::make_unique<prefix::non_simd::bin<25> >()};
  bin->insert(3, 2);
  bool test{bin->query(3, 2)};
  return test;
}
