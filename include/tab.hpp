#pragma once
#ifndef BIPSY_CPPUTILS_TAB_HPP
#define BIPSY_CPPUTILS_TAB_HPP


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


#endif  // BIPSY_CPPUTILS_TAB_HPP
