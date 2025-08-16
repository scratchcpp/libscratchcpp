// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <type_traits>

namespace libscratchcpp
{

// https://andreasfertig.com/blog/2024/01/cpp20-concepts-applied
// OR operator |
template<typename T>
constexpr std::enable_if_t<std::conjunction_v<std::is_enum<T>, std::is_same<bool, decltype(enable_enum_bitmask(std::declval<T>()))>>, T> operator|(const T lhs, const T rhs)
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

// OR assignment operator |=
template<typename T>
constexpr std::enable_if_t<std::conjunction_v<std::is_enum<T>, std::is_same<bool, decltype(enable_enum_bitmask(std::declval<T>()))>>, T &> operator|=(T &lhs, const T rhs)
{
    using underlying = std::underlying_type_t<T>;
    lhs = static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    return lhs;
}

// AND operator &
template<typename T>
constexpr std::enable_if_t<std::conjunction_v<std::is_enum<T>, std::is_same<bool, decltype(enable_enum_bitmask(std::declval<T>()))>>, T> operator&(const T lhs, const T rhs)
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

// AND assignment operator &=
template<typename T>
constexpr std::enable_if_t<std::conjunction_v<std::is_enum<T>, std::is_same<bool, decltype(enable_enum_bitmask(std::declval<T>()))>>, T &> operator&=(T &lhs, const T rhs)
{
    using underlying = std::underlying_type_t<T>;
    lhs = static_cast<T>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    return lhs;
}

// NOT operator ~
template<typename T>
constexpr std::enable_if_t<std::conjunction_v<std::is_enum<T>, std::is_same<bool, decltype(enable_enum_bitmask(std::declval<T>()))>>, T> operator~(const T value)
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(~static_cast<underlying>(value));
}

} // namespace libscratchcpp
