/**
 * @file SDL3_utils.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief A few utilities for SDL, primarily logging.
 *
 * This defines a suggested default clear color for the game.
 * It also provides an `asset_dir("asset_name.png")` function for
 * retrieving the paths of assets file names in a system-independent manner.
 * It also defines an `std::formatter` overload for `SDL_AppResult` for
 * use within format arguments in `std::format`.
 *
 * The primary utlity this header contains is the `Log` singleton static class.
 * This provides a static class method API for logging messages of any
 * priority level, using C++20's std::format specification instead of SDL's
 * old C-style printf format specification.
 *
 * It also contains an automatic logger indentation mechanism using
 * frame stack tracing. Indentation can be modified manually using
 * `Log::increase_indent()` and `Log::decrease_indent()`, but it can also be
 * automatically managed using RAII with automatic trace logging for a
 * frame call stack by using the `LOG_FRAME()` and `LOG_FRAME_CLASS(ClassName)`
 * macros.
 *
 * @version a_0.0.1-pre
 * @date 2026-05-17
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
#ifndef BIPSY_UTILS_SDL3_HPP
#define BIPSY_UTILS_SDL3_HPP


// #region Library Includes

#include "tab.hpp"
#include "formatter.hpp"

#include "SDL3/SDL.h"   // IWYU pragma: keep SDL_Color, SDL_LogMessage...

#include <string>       // std::string
#include <string_view>  // std::string_view
#include <utility>      // std::forward


// #endregion


namespace bipsy::sdl3_utils
{


// #region Constants (default color)

/**
 * @brief A default clear color for the game (sky blue)
 */
constexpr SDL_Color GAME_CLEAR_COLOR_DEFAULT = {0, 128, 255, 255};


// #endregion
// #region Functions (asset management)


/**
 * @brief Returns the full path to an asset file, given its name.
 *
 * On Android, assets are accessed relative to the APK's asset directory,
 * but on other platforms, we return SDL's base path
 * plus the "assets/" subdirectory.
 *
 * @param asset_name The name of the asset file within the asset directory
 * @return constexpr std::string
 */
inline constexpr std::string asset_dir(std::string asset_name)
{
  // get the asset directory
  return
#if not __ANDROID__
          std::string(SDL_GetBasePath()) + "assets/" + asset_name;
#else
          asset_name;  // On Android, we can just use the asset name directly
#endif  // __ANDROID__
}


// #endregion

/**
 * @brief Singleton class for static SDL logging utilities.
 *
 * Provides a public logging API that uses format strings, the same way
 * `std::format` would. This exposes static template methods for each of the SDL
 * logging priority levels:
 * `trace`, `verbose`, `debug`, `info`, `warn`, `error`, `critical`.
 * ```
 * Log::critical("Unrecoverable error! x = {}. Error {}", x, SDL_GetError());
 * ```
 *
 * Each with an overload that also takes an exact indentation to apply instead
 * of the one the singleton saves in memory. This can also be used to
 * offset indentation for a log message:
 * ```
 * Log::info(Log::indent() + 1, "This line is indented one more than normal!");
 * ```
 *
 * @note Uses the Meyers Singleton pattern.
 */
class Log
{

  // #region Static Class Data Members
  /**
   * @brief Global flag to enable or disable info logging.
   *
   */
  static inline bool m_log_info_enabled = true;

  // #endregion
  // #region Instance Data Members

  /**
   * @brief The current indentation level for logging messages.
   *
   * Default indentation width is 2 spaces, but TODO: can be configured
   */
  size_t m_indent;

  // #endregion

public:
  // #region Inner Class Declaration (RAII logging implementation)
  // needs to be public for the macro to work

  /**
   * @brief RAII-managed automatic indentor, using constructor & destructor.
   *
   * Used to manage the indentation level for a block of messages.
   * The constructor increases the indentation level and the destructor
   * decreases the indentation level.
   * This allows us to use RAII to automatically an indentation within
   * a function call stack frame by using the automatic calling of the
   * destructor to exit indentation, regardless of where
   * we return from a function.
   *
   */
  class LogFrame
  {
    // the function name for this frame
    std::string_view m_fn_name;
    // optional class name for this frame
    std::string_view m_class_name;
    // save the amount we indent/dedent by
    size_t m_ind_amt;

  public:
    /**
     * @brief Increase indentation by a given amount, or 1 by default.
     */
    explicit LogFrame(std::string_view fn_name,
                      std::string_view class_name    = "",
                      size_t           indent_amount = 1)
    : m_fn_name{fn_name}, m_class_name{class_name}, m_ind_amt{indent_amount}
    {
      // this is formatted with a tab size of 2 in mind, can change later
      static_assert(bipsy::tab_impl::tab_size == 2);

      // print class name if given, or just function name
      if (m_class_name.length() > 0)
        Log::trace("+ Entered {}::{}", m_class_name, m_fn_name);
      else
        Log::trace("+ Entered {}", m_fn_name);

      // increase indentation level after printing to show + deeper in line
      Log::increase_indent(m_ind_amt);
    }

    /**
     * @brief Decrease indentation by the same amount we incraeased by.
     */
    ~LogFrame()
    {
      // decrease indentation level first to show - deeper in line
      Log::decrease_indent(m_ind_amt);

      // print class name if given, or just function name
      if (m_class_name.length() > 0)
        Log::trace("- Exiting {}::{}", m_class_name, m_fn_name);
      else
        Log::trace("- Exiting {}", m_fn_name);
    }

    // Prevent copying
    LogFrame(const LogFrame &)              = delete;
    LogFrame & operator =(const LogFrame &) = delete;

  };  // class LogFrame

  // #endregion
  // #region Macro Definitions (RAII logging public API)
    // #region Macro Helpers
  // helper macros for concatenation of line number
#define BIPSY_LOG_FRAME_CONCAT_IMPL(a, b) a##b
#define BIPSY_LOG_FRAME_CONCAT(a, b)      BIPSY_LOG_FRAME_CONCAT_IMPL(a, b)
    // #endregion
    // #region API Macros
// Another convenience macro to create a log frame for a class member method
#define LOG_FRAME_CLASS(Class)                                           \
  ::bipsy::sdl3_utils::Log::LogFrame BIPSY_LOG_FRAME_CONCAT(_log_frame_, \
                                                            __LINE__)(   \
          __func__, #Class                                               \
  )
// Convenience macro to create a log frame for a function
// uses ##__LINE__ to append line number to the variable name,
// and __func__ to feed in the function name as a string.
#define LOG_FRAME() LOG_FRAME_CLASS()
    // #endregion
  // #endregion
  // #region Method Declarations (public)
    // #region Static Class Methods

  static Log & instance()
  {
    // Guaranteed to be instantiated only once on first use
    static Log instance;
    // Also guaranteed to be destroyed properly when the program exits
    // ALSO guaranteed to be thread-safe in C++11 and later!
    return instance;
  }

    // #endregion
    // #region Prevent copying and assignment (singleton)
  Log(const Log &)              = delete;
  Log & operator =(const Log &) = delete;
    // #endregion
    // #region Indentation Control Static Class Methods

  /**
   * @brief Get a reference to the singleton's indentation level.
   *
   * @return size_t& reference
   */
  static size_t & indent() { return instance().m_indent; }

  /**
   * @brief Get the indentation level.
   *
   * @return size_t value
   */
  static size_t get_indent() { return instance().m_indent; }

  /**
   * @brief Set the indentation level.
   *
   * @param indent size_t
   */
  static void set_indent(size_t indent) { instance().m_indent = indent; }

  /**
   * @brief Increase the indentation level by a certain amount, or 1 by default.
   *
   * @param amount Default is 1
   */
  static void increase_indent(size_t amount = 1)
  { instance().m_indent += amount; }

  /**
   * @brief Decrease the indentation level by a certain amount, or 1 by default.
   *
   * @param amount Default is 1
   */
  static void decrease_indent(size_t amount = 1)
  {
    instance().m_indent = (instance().m_indent >= amount) ?
                                  instance().m_indent - amount :
                                  0;
  }

  /**
   * @brief Reset the indentation level to 0.
   */
  static void reset_indent() { instance().m_indent = 0; }

    // #endregion
    // #region Logging API: Template Logging Static Methods (trace/verbose/debug/info/warn/error/critical)

  /**
   * @brief Log a message of given priority, with optional indentation level.
   *
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global
   * indentation level.
   * @param message A std::format-style format string for the log message.
   * @param args Format arguments for the log message.
   */
  template <typename... Args>
  static inline void log(SDL_LogPriority             priority,
                         size_t                      indent,
                         std::format_string<Args...> message,
                         Args &&... args)
  {
    SDL_LogIndent(indent,
                  priority,
                  std::format(message, std::forward<Args>(args)...));
  }

  template <typename... Args>
  static inline void log(SDL_LogPriority             priority,
                         std::format_string<Args...> message,
                         Args &&... args)
  { log(priority, instance().indent(), message, std::forward<Args>(args)...); }

  /**
   * @brief Log a TRACE-level message, with optional indentation level.
   *
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global
   * indentation level.
   * @param message A std::format-style format string for the log message.
   * @param args Format arguments for the log message.
   */
  template <typename... Args>
  static inline void trace(size_t                      indent,
                           std::format_string<Args...> message,
                           Args &&... args)
  {
    SDL_LogIndent(indent,
                  SDL_LogPriority::SDL_LOG_PRIORITY_TRACE,
                  std::format(message, std::forward<Args>(args)...));
  }

  template <typename... Args>
  static inline void trace(std::format_string<Args...> message, Args &&... args)
  { trace(instance().indent(), message, std::forward<Args>(args)...); }

  /**
   * @brief Log a VERBOSE-level message, with optional indentation level.
   *
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global
   * indentation level.
   * @param message A std::format-style format string for the log message.
   * @param args Format arguments for the log message.
   *
   * @warning see `trace` function's @@warning
   */
  template <typename... Args>
  static inline void verbose(size_t                      indent,
                             std::format_string<Args...> message,
                             Args &&... args)
  {
    SDL_LogIndent(indent,
                  SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE,
                  std::format(message, std::forward<Args>(args)...));
  }

  template <typename... Args>
  static inline void verbose(std::format_string<Args...> message,
                             Args &&... args)
  { verbose(instance().indent(), message, std::forward<Args>(args)...); }

  /**
   * @brief Log a DEBUG-level message, with optional indentation level.
   *
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global
   * indentation level.
   * @param message A std::format-style format string for the log message.
   * @param args Format arguments for the log message.
   *
   * @warning see `trace` function's @@warning
   */
  template <typename... Args>
  static inline void debug(size_t                      indent,
                           std::format_string<Args...> message,
                           Args &&... args)
  {
    SDL_LogIndent(indent,
                  SDL_LogPriority::SDL_LOG_PRIORITY_DEBUG,
                  std::format(message, std::forward<Args>(args)...));
  }

  template <typename... Args>
  static inline void debug(std::format_string<Args...> message, Args &&... args)
  { debug(instance().indent(), message, std::forward<Args>(args)...); }

  /**
   * @brief Log an INFO-level message, with optional indentation level.
   *
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global
   * indentation level.
   * @param message A std::format-style format string for the log message.
   * @param args Format arguments for the log message.
   *
   * @warning see `trace` function's @@warning
   */
  template <typename... Args>
  static inline void info(size_t                      indent,
                          std::format_string<Args...> message,
                          Args &&... args)
  {
    if (m_log_info_enabled)
    {
      SDL_LogIndent(indent,
                    SDL_LogPriority::SDL_LOG_PRIORITY_INFO,
                    std::format(message, std::forward<Args>(args)...));
    }
  }

  template <typename... Args>
  static inline void info(std::format_string<Args...> message, Args &&... args)
  { info(instance().indent(), message, std::forward<Args>(args)...); }

  /**
   * @brief Log a WARN-level message, with optional indentation level.
   *
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global
   * indentation level.
   * @param message A std::format-style format string for the log message.
   * @param args Format arguments for the log message.
   *
   * @return SDL_AppResult `SDL_APP_FAILURE` for convenience in error handling.
   *
   * @warning see `trace` function's @@warning
   */
  template <typename... Args>
  static inline SDL_AppResult warn(size_t                      indent,
                                   std::format_string<Args...> message,
                                   Args &&... args)
  {
    SDL_LogIndent(indent,
                  SDL_LogPriority::SDL_LOG_PRIORITY_WARN,
                  std::format(message, std::forward<Args>(args)...));
    return SDL_APP_FAILURE;
  }

  template <typename... Args>
  static inline SDL_AppResult warn(std::format_string<Args...> message,
                                   Args &&... args)
  { return warn(instance().indent(), message, std::forward<Args>(args)...); }

  /**
   * @brief Log a ERROR-level message, with optional indentation level.
   *
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global
   * indentation level.
   * @param message A std::format-style format string for the log message.
   * @param args Format arguments for the log message.
   *
   * @return SDL_AppResult `SDL_APP_FAILURE` for convenience in error handling.
   *
   * @warning see `trace` function's @@warning
   */
  template <typename... Args>
  static inline SDL_AppResult error(size_t                      indent,
                                    std::format_string<Args...> message,
                                    Args &&... args)
  {
    SDL_LogIndent(indent,
                  SDL_LogPriority::SDL_LOG_PRIORITY_ERROR,
                  std::format(message, std::forward<Args>(args)...));
    return SDL_APP_FAILURE;
  }

  template <typename... Args>
  static inline SDL_AppResult error(std::format_string<Args...> message,
                                    Args &&... args)
  { return error(instance().indent(), message, std::forward<Args>(args)...); }

  /**
   * @brief Log a CRITICAL-level message, with optional indentation level.
   *
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global
   * indentation level.
   * @param message A std::format-style format string for the log message.
   * @param args Format arguments for the log message.
   *
   * @return SDL_AppResult `SDL_APP_FAILURE` for convenience in error handling.
   *
   * @warning see `trace` function's @@warning
   */
  template <typename... Args>
  static inline SDL_AppResult critical(size_t                      indent,
                                       std::format_string<Args...> message,
                                       Args &&... args)
  {
    SDL_LogIndent(indent,
                  SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL,
                  std::format(message, std::forward<Args>(args)...));
    return SDL_APP_FAILURE;
  }

  template <typename... Args>
  static inline SDL_AppResult critical(std::format_string<Args...> message,
                                       Args &&... args)
  {
    return critical(instance().indent(), message, std::forward<Args>(args)...);
  }

    // #endregion
    // #region Public Utilities for Logging API


  /**
   * @brief Helper function to log an SDL error message for a specific
   *        subsystem initialization failure.
   *
   * @param subsystem The name of the subsystem that failed to initialize.
   *
   * @return SDL_AppResult SDL_APP_FAILURE for convenience.
   */
  // template <typename... Args>
  static inline SDL_AppResult error_init(std::string subsystem)
  {
    error("Failed to initialize {}: {}", subsystem, SDL_GetError());
    return SDL_APP_FAILURE;
  }

  /**
   * @brief Enables or disables info logging.
   */
  static inline constexpr void info_enable(bool enable = true)
  { m_log_info_enabled = enable; }

  static inline constexpr void        info_disable() { info_enable(false); }

  static inline constexpr std::string get_log_priority_name(
          SDL_LogPriority priority
  )
  {
    switch (priority)
    {
#define CASE(PRIORITY)                                \
  case SDL_LOG_PRIORITY_##PRIORITY: return #PRIORITY;
      CASE(TRACE)
      CASE(VERBOSE)
      CASE(DEBUG)
      CASE(INFO)
      CASE(WARN)
      CASE(ERROR)
      CASE(CRITICAL)
      CASE(INVALID)
      CASE(COUNT)
#undef CASE
    default: return "UNKNOWN";
    }
  }


    // #endregion
  // #endregion


private:

  // #region Private Implementation

  // Prevent instantiation from outside the class
  Log()
  : m_indent{0}
  {}

  /**
   * @brief Logs an SDL info message with specified indentation level.
   *
   * Uses std::format to produce a final message and logs it as plain text,
   * with indentation applied using the `tab` function from `tab.hpp`.
   *
   * @param indent The number of indentation levels to apply, two spaces each.
   *               `indent * 2 == space_count`.
   * @param priority The SDL_LogPriority level for the message
   *                 (e.g. SDL_LOG_PRIORITY_INFO).
   * @param message The final message text to log.
   */
  static inline void SDL_LogIndent(size_t           indent,
                                   SDL_LogPriority  priority,
                                   std::string_view message)
  {
    const std::string indented = bipsy::tab(indent) + std::string(message);

    SDL_LogMessage(
            SDL_LOG_CATEGORY_APPLICATION, priority, "%s", indented.c_str()
    );
  }

  // #endregion


};  // class Log


}  // namespace bipsy::sdl3_utils


// #region Formatting Compatibility: `std::formatter` Specialization

// must be defined in global namespace
REGISTER_FORMATTER_ENUM(SDL_AppResult);


// #endregion

#endif  // BIPSY_UTILS_SDL3_HPP
