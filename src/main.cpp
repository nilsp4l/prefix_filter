#include "prefix/non_simd/prefix_filter.hpp"
#include "prefix/simd/pocket_dictionary.hpp"
#include <iostream>

int main()
{

  auto pd{prefix::simd::pocket_dictionary<25>()};
  
  
  pd.insert(2, 3);
  pd.insert(1, 2);
  
  std::cout << std::endl;
  pd.insert(1, 4);
  pd.insert(5, 9);
  
  pd.insert(2, 5);
  
  pd.insert(2, 6);
  pd.insert(0, 11);
  pd.insert(0, 12);
  pd.insert(0, 13);
  pd.insert(0, 14);

  pd.insert(0, 15);
  pd.insert(0, 16);  

  for(uint8_t i{0}; i < 25; ++i)
  {
    std::cout << std::to_string(pd[i]) << std::endl;
  }

  std::cout << (pd.query(5, 9) ? "true" : "false") << std::endl;


  return 0;
}
