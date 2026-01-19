/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <memory>
#include <utility>
#include <type_traits>
#include <cstddef>

#if __cplusplus < 201402L
namespace std {

template <class T, class... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <class T>
typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, std::unique_ptr<T>>::type make_unique(
    std::size_t n)
{
    using U = typename std::remove_extent<T>::type;
    return std::unique_ptr<T>(new U[n]());
}

template <class T, class... Args>
typename std::enable_if<(std::extent<T>::value != 0), void>::type make_unique(Args&&...) = delete;

}  // namespace std
#endif
