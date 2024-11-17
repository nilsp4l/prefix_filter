//
// Created by nils on 11/15/24.
//

#ifndef I_BIN_HPP
#define I_BIN_HPP

#include <cstdint>

namespace prefix::interfaces {
    class i_bin {
    public:
        virtual ~i_bin() = default;

        virtual uint8_t &operator[](uint8_t index) const = 0;

        [[nodiscard]] constexpr virtual bool query(const uint8_t q, const uint8_t r) const = 0;

        constexpr virtual void insert(const uint8_t q, const uint8_t r) = 0;
    };
}// namespace prefix::interfaces

#endif //I_BIN_HPP
