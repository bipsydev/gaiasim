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
 * @param TypeName The input type name to create a formatter for.
 * @param OutputFormName The output type, TypeName inherits its formatting.
 */
#define FORMATTER(TypeName, OutputFormName)                                    \
  template <>                                                                  \
  struct std::formatter<TypeName, char> : std::formatter<OutputFormName, char> \
  {                                                                            \
    auto format(TypeName init_request, std::format_context & ctx) const        \
    {                                                                          \
      return std::formatter<OutputFormName, char>::format(                     \
              static_cast<OutputFormName>(init_request), ctx                   \
      );                                                                       \
    }                                                                          \
  };


/**
 * @brief Create an std::formatter specialization for an enumeration.
 *
 * @warning The enumeration MUST have a template specialization for
 *          `bipsy::to_string`!
 *
 */
#define FORMATTER_ENUM(EnumType)                                     \
  template <>                                                        \
  struct std::formatter<EnumType> : std::formatter<std::string_view> \
  {                                                                  \
    auto format(EnumType e, std::format_context & ctx) const         \
    {                                                                \
      return std::formatter<std::string_view>::format(               \
              magic_enum::enum_name(e), ctx                          \
      );                                                             \
    }                                                                \
  };


#endif  // BIPSY_UTILS_STDCPP_FORMATTER_HPP
