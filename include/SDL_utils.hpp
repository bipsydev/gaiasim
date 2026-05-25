#pragma once
#ifndef BIPSY_SDL3UTILS_SDL_LOGINDENT_HPP
#define BIPSY_SDL3UTILS_SDL_LOGINDENT_HPP

#include "tab.hpp"

#include "SDL3/SDL.h"

#include <string>   // std::string
#include <utility>  // std::forward


namespace bipsy::sdlutils
{

  // Default clear color (Sky Blue)
  constexpr SDL_Color GAME_CLEAR_COLOR_DEFAULT = {0, 128, 255, 255}; 
/**
 * @brief Logs an SDL info message with specified indentation level.
 * 
 * Uses `SDL_LogMessageV` to log the message with variable arguments and applies
 * indentation using the `tab` function from `tab.hpp`.
 * 
 * @param indent The number of indentation levels to apply, two spaces each.
 *               `indent * 2 == space_count`.
 * @param priority The SDL_LogPriority level for the message
 *                 (e.g. SDL_LOG_PRIORITY_INFO).
 * @param message The message format string (printf-style) to log.
 * @param args The variadic arguments to format into the message.
 *             (parameter pack)
 */
template <typename... Args>
inline void SDL_LogIndent(int indent, SDL_LogPriority priority,
                          // `&&` is an rvalue reference, which allows
                          // perfect forwarding of template arguments
                          const char* message, Args&&... args)
{
  SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
                 priority,
                 (bipsy::tab(indent) + message).c_str(),
                 // forward the variadic arguments to preserve & reduce copying
                 std::forward<Args>(args)...);
}

template <typename... Args>
inline void SDL_LogTraceIndent(int indent, const char* message, Args&&... args)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_TRACE,
    message, std::forward<Args>(args)...);
}

template <typename... Args>
inline void SDL_LogVerboseIndent(int indent, const char* message, Args&&... args)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_VERBOSE,
    message, std::forward<Args>(args)...);
}

template <typename... Args>
inline void SDL_LogDebugIndent(int indent, const char* message, Args&&... args)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_DEBUG,
    message, std::forward<Args>(args)...);
}

template <typename... Args>
inline void SDL_LogInfoIndent(int indent, const char* message, Args&&... args)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_INFO,
    message, std::forward<Args>(args)...);
}

template <typename... Args>
inline void SDL_LogWarnIndent(int indent, const char* message, Args&&... args)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_WARN,
    message, std::forward<Args>(args)...);
}

template <typename... Args>
inline void SDL_LogErrorIndent(int indent, const char* message, Args&&... args)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_ERROR,
    message, std::forward<Args>(args)...);
}

template <typename... Args>
inline void SDL_LogCriticalIndent(int indent, const char* message, Args&&... args)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_CRITICAL,
    message, std::forward<Args>(args)...);
}


/**
 * @brief Helper function to log an SDL error message for a specific
 *        subsystem initialization failure.
 * 
 * @param subsystem The name of the subsystem that failed to initialize.
 * @param indent The number of indentation levels to apply.
 * 
 * @return SDL_AppResult SDL_APP_FAILURE for convenience.
 */
template <typename... Args>
inline SDL_AppResult log_error_init(std::string subsystem, int indent = 0,
  Args&&... args)
{
  SDL_LogErrorIndent(indent, "Failed to initialize %s: %s",
    subsystem.c_str(), SDL_GetError(), std::forward<Args>(args)...);
  return SDL_APP_FAILURE;
}

/**
 * @brief Global flag to enable or disable info logging.
 * 
 */
static bool log_info_enabled = true;

/**
 * @brief Enables or disables info logging.
 */
inline constexpr void log_info_enable(bool enable = true)
{ log_info_enabled = enable; }

inline constexpr void log_info_disable()
{ log_info_enable(false); }

/**
 * @brief Log a TRACE-level message, with optional indentation level.
 * 
 * @tparam Args Variadic template parameters for the format arguments.
 * @param message A printf-style format string for the log message.
 * @param indent Number of indentation levels to apply. Omit for 0 indentation.
 * @param args Format arguments for the log message.
 */
template <typename... Args>
inline void log_trace(std::string message, int indent, Args&&... args)
{
  SDL_LogTraceIndent(indent, message.c_str(), std::forward<Args>(args)...);
}

template <typename... Args>
inline void log_trace(std::string message, Args&&... args)
{
  log_trace(message, 0, std::forward<Args>(args)...);
}

/**
 * @brief Log a VERBOSE-level message, with optional indentation level.
 * 
 * @tparam Args Variadic template parameters for the format arguments.
 * @param message A printf-style format string for the log message.
 * @param indent Number of indentation levels to apply. Omit for 0 indentation.
 * @param args Format arguments for the log message.
 */
template <typename... Args>
inline void log_verbose(std::string message, int indent, Args&&... args)
{
  SDL_LogVerboseIndent(indent, message.c_str(), std::forward<Args>(args)...);
}

template <typename... Args>
inline void log_verbose(std::string message, Args&&... args)
{
  log_verbose(message, 0, std::forward<Args>(args)...);
}

/**
 * @brief Log a DEBUG-level message, with optional indentation level.
 * 
 * @tparam Args Variadic template parameters for the format arguments.
 * @param message A printf-style format string for the log message.
 * @param indent Number of indentation levels to apply. Omit for 0 indentation.
 * @param args Format arguments for the log message.
 */
template <typename... Args>
inline void log_debug(std::string message, int indent, Args&&... args)
{
  SDL_LogDebugIndent(indent, message.c_str(), std::forward<Args>(args)...);
}

template <typename... Args>
inline void log_debug(std::string message, Args&&... args)
{
  log_debug(message, 0, std::forward<Args>(args)...);
}

/**
 * @brief Log an INFO-level message, with optional indentation level.
 * 
 * @tparam Args Variadic template parameters for the format arguments.
 * @param message A printf-style format string for the log message.
 * @param indent Number of indentation levels to apply. Omit for 0 indentation.
 * @param args Format arguments for the log message.
 */
template <typename... Args>
inline void log_info(std::string message, int indent, Args&&... args)
{
  if (log_info_enabled)
  {
    SDL_LogInfoIndent(indent, message.c_str(), std::forward<Args>(args)...);
  }
}

template <typename... Args>
inline void log_info(std::string message, Args&&... args)
{
  log_info(message, 0, std::forward<Args>(args)...);
}

/**
 * @brief Log a WARN-level message, with optional indentation level.
 * 
 * @tparam Args Variadic template parameters for the format arguments.
 * @param message A printf-style format string for the log message.
 * @param indent Number of indentation levels to apply. Omit for 0 indentation.
 * @param args Format arguments for the log message.
 * 
 * @return SDL_AppResult `SDL_APP_FAILURE` for convenience in error handling.
 */
template <typename... Args>
inline SDL_AppResult log_warn(std::string message, int indent, Args&&... args)
{
  SDL_LogWarnIndent(indent, message.c_str(), std::forward<Args>(args)...);
  return SDL_APP_FAILURE;
}

template <typename... Args>
inline SDL_AppResult log_warn(std::string message, Args&&... args)
{
  return log_warn(message, 0, std::forward<Args>(args)...);
}

/**
 * @brief Log a ERROR-level message, with optional indentation level.
 * 
 * @tparam Args Variadic template parameters for the format arguments.
 * @param message A printf-style format string for the log message.
 * @param indent Number of indentation levels to apply. Omit for 0 indentation.
 * @param args Format arguments for the log message.
 * 
 * @return SDL_AppResult `SDL_APP_FAILURE` for convenience in error handling.
 */
template <typename... Args>
inline SDL_AppResult log_error(std::string message, int indent, Args&&... args)
{
  SDL_LogErrorIndent(indent, message.c_str(), std::forward<Args>(args)...);
  return SDL_APP_FAILURE;
}

template <typename... Args>
inline SDL_AppResult log_error(std::string message, Args&&... args)
{
  return log_error(message, 0, std::forward<Args>(args)...);
}

/**
 * @brief Log a CRITICAL-level message, with optional indentation level.
 * 
 * @tparam Args Variadic template parameters for the format arguments.
 * @param message A printf-style format string for the log message.
 * @param indent Number of indentation levels to apply. Omit for 0 indentation.
 * @param args Format arguments for the log message.
 * 
 * @return SDL_AppResult `SDL_APP_FAILURE` for convenience in error handling.
 */
template <typename... Args>
inline SDL_AppResult log_critical(std::string message, int indent, Args&&... args)
{
  SDL_LogCriticalIndent(indent, message.c_str(), std::forward<Args>(args)...);
  return SDL_APP_FAILURE;
}

template <typename... Args>
inline SDL_AppResult log_critical(std::string message, Args&&... args)
{
  return log_critical(message, 0, std::forward<Args>(args)...);
}


inline constexpr std::string get_log_priority_name(SDL_LogPriority priority)
{
  switch (priority)
  {
#define CASE(PRIORITY) \
    case SDL_LOG_PRIORITY_ ##PRIORITY: return #PRIORITY;
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



inline constexpr std::string asset_dir(std::string asset_name)
{
  // get the asset directory
  return
#if not __ANDROID__
  std::string(SDL_GetBasePath()) + "assets/";
#else
  ""; // On Android, we can just use the asset name directly
#endif // __ANDROID__
}


} // namespace bipsy::sdlutils


#endif // BIPSY_SDL3UTILS_SDL_LOGINDENT_HPP