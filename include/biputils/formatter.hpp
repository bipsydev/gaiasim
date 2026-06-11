/**
 * @file formatter_spec.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Defines a FORMATTER macro that creates a std::formatter
 *        specialization given a type and it's output
 *        form type that's used in the string.
 *
 * @version 0.1
 * @date 2026-06-10
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#ifndef BIPSY_UTILS_STDCPP_FORMATTER_HPP
#define BIPSY_UTILS_STDCPP_FORMATTER_HPP


#include "magic_enum/magic_enum.hpp"  // IWYU pragma: keep (it's in the macro)

#include <format>                     // IWYU pragma: keep


/**
 * @brief Create an std::formatter specialization for the type and output.
 *
 * Creates a formatter by inheriting the formatter of an output type.
 * The formatting input type is `static_cast`ed to the inherited output type.
 *
 * @param InputType The input type name to create a formatter for.
 * @param CastType The output type name, `InputType` inherits its formatting.
 *
 * @warning This macro MUST be called in global scope, outside of any namespace!
 *          It's recommended to place `FORMATTER(InputType, CastType)` at the
 *          bottom of the header that the enumeration is declared in
 *          in global scope.
 */
#define FORMATTER(InputType, CastType)                                    \
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
 *          It's recommended to place `FORMATTER_ENUM(EnumName)` at the bottom
 *          of the header that the enumeration is declared in, in global scope.
 */
#define FORMATTER_ENUM(EnumType)                                     \
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


#endif  // BIPSY_UTILS_STDCPP_FORMATTER_HPP
