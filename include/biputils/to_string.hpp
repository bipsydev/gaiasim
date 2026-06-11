#pragma once
#ifndef BIPSY_UTILS_STDCPP_TO_STRING_HPP


#include <string_view>


namespace bipsy
{

/**
 * @brief Specialize this template to create a to_string function for any type.
 * 
 * @tparam TypeName The type to specialize a `to_string` function for.
 * @param result The argument we are converting to string.
 * @return std::string_view the read-only string view of the string.
 */
template <typename TypeName>
std::string_view to_string(TypeName result) = delete;

}


#endif  // BIPSY_UTILS_STDCPP_TO_STRING_HPP
