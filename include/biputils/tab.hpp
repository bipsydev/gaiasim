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


// #region Library Includes

#include <cstddef>  // size_t
#include <string>
#include <cassert>


// #endregion


namespace bipsy
{

// #region Implementation details for `tab` function

namespace tab_impl
{
// The character to render as "spaces" for tabs
inline constexpr char tab_char    = ' ';

// Number of tab_chars per tab
inline constexpr size_t tab_size  = 2;

// The maximum depth (for compile-time tab generation)
inline constexpr size_t max_depth = 64;

// The pre-generated padding that can fit the max_depth
inline const std::string padding(max_depth * tab_size, tab_char);
}  // namespace tab_impl


// #endregion
// #region Tab-generating functions

/**
 * @brief Generates `n` tabs at runtime, rendered as space characters.
 *
 * @param n The number of tabs to render as spaces,
            based on `tab_impl::tab_size`.
 * @return std::string The output rendered string of spaces.
 */
inline std::string_view tab(const size_t n = 1) noexcept
{
  assert(n < tab_impl::tab_size * tab_impl::max_depth);

  // get a view of the padding
  auto view = std::string_view(tab_impl::padding);
  // return as much of it as we can
  return (n * tab_impl::tab_size < tab_impl::padding.length()) ?
                 view.substr(0, n * tab_impl::tab_size) :
                 view;  // Just returns max if we asked for more than padding
}


}  // namespace bipsy


// #endregion

#endif  // BIPSY_UTILS_STDCPP_TAB_HPP
