#include "prefix/non_simd/prefix_filter.hpp"
#include "prefix/adapted/bin.hpp"

int main()
{

  auto bin{prefix::adapted::bin()};
  bin.insert(2);
  return 0;
}
