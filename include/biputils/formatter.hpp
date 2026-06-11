/**
 * @file formatter_spec.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Defines a FORMATTER macro that creates a std::formatter
 *        specialization given a type and it's output
 *        form type that's used in the string.
 *
 * @version a_0.0.1-pre
 * @date 2026-06-10
 *
 * @copyright Copyright (c) 2026
 *
 * @section License
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 */
#pragma once
#ifndef BIPSY_UTILS_STDCPP_FORMATTER_HPP
#define BIPSY_UTILS_STDCPP_FORMATTER_HPP


// #region Library Includes

#include "magic_enum/magic_enum.hpp"  // IWYU pragma: keep (it's in the macro)

#include <format>                     // IWYU pragma: keep


// #endregion
// #region Macro Definitions

namespace bipsy
{

/**
 * @brief Create an inherited std::formatter specialization for the type.
 *
 * Creates a formatter by inheriting the formatter of an output type.
 * The formatting input type is `static_cast`ed to the inherited output type.
 *
 * @param InputType The input type name to create a formatter for.
 * @param CastType The output type name, `InputType` inherits its formatting.
 *
 * @warning This macro MUST be called in global scope, outside of any namespace!
 *          It's recommended to place `REGISTER_FORMATTER(InputType, CastType)`
 *          at the bottom of the header that the type is declared in.
 */
// #tag Macro:REGISTER_FORMATTER
#define REGISTER_FORMATTER(InputType, CastType)                           \
  template <>                                                             \
  struct std::formatter<InputType, char> : std::formatter<CastType, char> \
  {                                                                       \
    auto format(InputType input, std::format_context & ctx) const         \
    {                                                                     \
      return std::formatter<CastType, char>::format(                      \
              static_cast<CastType>(input), ctx                           \
      );                                                                  \
    }                                                                     \
  };


/**
 * @brief Create an std::formatter specialization for an enumeration.
 *
 * Uses magic_enum to get the name of the enum value for us.
 *
 * @param EnumType the input enumeration type name to create a formatter for.
 *
 * @warning This macro MUST be called in global scope, outside of any namespace!
 *          It's recommended to place `REGISTER_FORMATTER_ENUM(EnumName)`
 *          at the bottom of the header that the enumeration is declared in.
 */
#define REGISTER_FORMATTER_ENUM(EnumType)                            \
  template <>                                                        \
  struct std::formatter<EnumType> : std::formatter<std::string_view> \
  {                                                                  \
    auto format(EnumType input, std::format_context & ctx) const     \
    {                                                                \
      return std::formatter<std::string_view>::format(               \
              magic_enum::enum_name(input), ctx                      \
      );                                                             \
    }                                                                \
  };


}  // namespace bipsy


// #endregion

#endif  // BIPSY_UTILS_STDCPP_FORMATTER_HPP
