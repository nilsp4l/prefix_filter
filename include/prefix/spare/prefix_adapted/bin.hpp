//
// Created by nils on 1/21/25.
//

#ifndef PREFIX_SPARE_PREFIX_ADAPTED_BIN_HPP
#define PREFIX_SPARE_PREFIX_ADAPTED_BIN_HPP

#include <cstdint>


namespace prefix::spare::prefix_adapted
{
struct bin
{
public:

  static constexpr std::size_t maximum_size{31};

  static bool query(uint8_t r, uint8_t* data);

  static void insert(uint8_t r, uint8_t* data);

  static uint8_t size(uint8_t* data);


private:
  static void increase_size(uint8_t* data);
};
}// namespace prefix::spare


#endif //PREFIX_SPARE_PREFIX_ADAPTED_BIN_HPP
