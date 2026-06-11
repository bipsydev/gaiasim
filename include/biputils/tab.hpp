/**
 * @file tab.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Basic # of tabs -> string generation based on desired tab width.
 *
 * @version a_0.0.1-pre
 * @date 2026-02-12
 *
 * @copyright Copyright (c) 2026
 *
 * @section License
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 **/
#pragma once
#ifndef BIPSY_UTILS_STDCPP_TAB_HPP
#define BIPSY_UTILS_STDCPP_TAB_HPP


#include <cstddef>  // size_t
#include <string>


namespace bipsy
{


namespace tab_impl
{
inline constexpr char   tab_char = ' ';
inline constexpr size_t tab_size = 2;  // Number of tab_chars per tab
}  // namespace tab_impl


inline std::string tab(const size_t n = 1) noexcept
{ return std::string(n * tab_impl::tab_size, tab_impl::tab_char); }

inline char * tab_c(const size_t n = 1) noexcept
{
  static std::string tab_str = tab(n);
  return tab_str.data();
}

}  // namespace bipsy


#endif  // BIPSY_UTILS_STDCPP_TAB_HPP
