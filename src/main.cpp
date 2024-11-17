#include <memory>
#include <iostream>

#include "prefix/non_simd/bin.hpp"
#include "prefix/interfaces/i_bin.hpp"

int main()
{
  std::unique_ptr<prefix::interfaces::i_bin> bin{std::make_unique<prefix::non_simd::bin<25> >()};
  for(int i{0}; i < 25; ++i)
  {

     bin->insert(i, i);
     for(uint8_t j{0}; j < 25; ++j)
     {
        uint8_t val{(*bin)[j]};
        std::cout << std::to_string(val) << std::endl;
     }
  }
  for(int i{0}; i < 25; ++i)
  {
     bool test{bin->query(i, i)};
     std::cout << (test ? "true" : "false") << std::endl;
  }


  return 0;
}
