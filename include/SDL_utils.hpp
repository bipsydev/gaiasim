#pragma once
#ifndef BIPSY_SDL3UTILS_SDL_LOGINDENT_HPP
#define BIPSY_SDL3UTILS_SDL_LOGINDENT_HPP

#include "tab.hpp"

#include "SDL3/SDL.h"

#include <string>


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
 * @param priority The SDL_LogPriority level for the message (e.g. SDL_LOG_PRIORITY_INFO).
 * @param message The message format string (printf-style) to log.
 * @param ... These are the format arguments for the message,
 *            used if the message string contains format specifiers (e.g. `%s`).
 */
inline void SDL_LogIndent(int indent, SDL_LogPriority priority, const char* message, ...)
{
  va_list args_list;

  va_start(args_list, message);
  SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                  priority,
                  (bipsy::tab(indent) + message).c_str(),
                  args_list);
  va_end(args_list);
}

inline void SDL_LogTraceIndent(int indent, const char* message, ...)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_TRACE, message);
}
inline void SDL_LogVerboseIndent(int indent, const char* message, ...)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_VERBOSE, message);
}
inline void SDL_LogDebugIndent(int indent, const char* message, ...)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_DEBUG, message);
}
inline void SDL_LogInfoIndent(int indent, const char* message, ...)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_INFO, message);
}
inline void SDL_LogWarnIndent(int indent, const char* message, ...)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_WARN, message);
}
inline void SDL_LogErrorIndent(int indent, const char* message, ...)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_ERROR, message);
}
inline void SDL_LogCriticalIndent(int indent, const char* message, ...)
{
  SDL_LogIndent(indent, SDL_LOG_PRIORITY_CRITICAL, message);
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
inline SDL_AppResult log_error_init(std::string subsystem, int indent = 0)
{
  SDL_LogErrorIndent(indent, "Failed to initialize %s: %s",
    subsystem.c_str(), SDL_GetError());
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



inline void log_trace(std::string message, int indent = 0)
{
  SDL_LogTraceIndent(indent, message.c_str());
}

inline void log_verbose(std::string message, int indent = 0)
{
  SDL_LogVerboseIndent(indent, message.c_str());
}

inline void log_debug(std::string message, int indent = 0)
{
  SDL_LogDebugIndent(indent, message.c_str());
}

/**
 * @brief Logs an SDL info message with optional indentation level.
 * 
 * Only prints if `log_info_enabled` is true.
 * Uses `SDL_LogIndent` to log the message with the specified indentation level.
 * 
 * @param message The message to log.
 * @param indent The number of indentation levels to apply.
 */
inline void log_info(std::string message, int indent = 0)
{
  if (log_info_enabled)
  {
    SDL_LogInfoIndent(indent, message.c_str());
  }
}

inline SDL_AppResult log_warn(std::string message, int indent = 0)
{
  SDL_LogWarnIndent(indent, message.c_str());
  return SDL_APP_FAILURE;
}

/**
 * @brief Logs an SDL error message.
 * 
 * @param message The error message to log.
 * @param indent The number of indentation levels to apply.
 * 
 * @return SDL_AppResult SDL_APP_FAILURE for convenience in error handling.
 */
inline SDL_AppResult log_error(std::string message, int indent = 0)
{
  SDL_LogErrorIndent(indent, message.c_str());
  return SDL_APP_FAILURE;
}

inline SDL_AppResult log_critical(std::string message, int indent = 0)
{
  SDL_LogCriticalIndent(indent, message.c_str());
  return SDL_APP_FAILURE;
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