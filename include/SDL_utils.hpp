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
 * @param message The message format string (printf-style) to log.
 * @param ... These are the format arguments for the message,
 *            used if the message string contains format specifiers (e.g. `%s`).
 */
inline void SDL_LogIndent(int indent, const char* message, ...)
{
  va_list args_list;

  va_start(args_list, message);
  SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                  SDL_LOG_PRIORITY_INFO,
                  (bipsy::tab(indent) + message).c_str(),
                  args_list);
  va_end(args_list);
}


/**
 * @brief Helper function to log an SDL error message for a specific
 *        subsystem initialization failure.
 * 
 * @param subsystem The name of the subsystem that failed to initialize.
 * @return SDL_AppResult SDL_APP_FAILURE for convenience.
 */
inline SDL_AppResult log_error_init(std::string subsystem)
{
  SDL_LogError(SDL_LOG_CATEGORY_ERROR,
    "Failed to initialize %s: %s", subsystem.c_str(), SDL_GetError());
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
    SDL_LogIndent(indent, message.c_str());
  }
}

/**
 * @brief Logs an SDL error message.
 * 
 * @param message The error message to log.
 * 
 * @return SDL_AppResult SDL_APP_FAILURE for convenience in error handling.
 */
inline SDL_AppResult log_error(std::string message)
{
  SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", message.c_str());
  return SDL_APP_FAILURE;
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