#pragma once
#ifndef BIPSY_SDL3_UTILS_HPP
#define BIPSY_SDL3_UTILS_HPP

#include "tab.hpp"

#include "SDL3/SDL.h"

#include <string>   // std::string
#include <utility>  // std::forward


namespace bipsy::sdl3_utils
{

/**
 * @brief A default clear color for the game (sky blue)
 */
constexpr SDL_Color GAME_CLEAR_COLOR_DEFAULT = {0, 128, 255, 255};


/**
 * @brief Returns the full path to an asset file, given its name.
 * 
 * On Android, assets are accessed relative to the APK's asset directory,
 * but on other platforms, we return SDL's base path
 * plus the "assets/" subdirectory.
 * 
 * @param asset_name 
 * @return constexpr std::string 
 */
inline constexpr std::string asset_dir(std::string asset_name)
{
  // get the asset directory
  return
#if not __ANDROID__
  std::string(SDL_GetBasePath()) + "assets/" + asset_name;
#else
  asset_name; // On Android, we can just use the asset name directly
#endif // __ANDROID__
}


/**
 * @brief Singleton class for SDL logging utilities.
 * 
 * Uses the Meyers Singleton pattern.
 * 
 */
class Log
{

  /**
   * @brief Global flag to enable or disable info logging.
   * 
   */
  static inline bool log_info_enabled = true;

  /**
   * @brief The current indentation level for logging messages.
   * 
   * Default indentation width is 2 spaces, but TODO: can be configured
   */
  size_t m_indent;

public:
  static Log& instance()
  {
    // Guaranteed to be instantiated only once on first use
    static Log instance;
    // Also guaranteed to be destroyed properly when the program exits
    // ALSO guaranteed to be thread-safe in C++11 and later!
    return instance;
  }

  // Prevent copying and assignment (singleton)
  Log(const Log&) = delete;
  Log& operator=(const Log&) = delete;


  // ---- Indentation control functions ----

  /**
   * @brief Get a reference to the singleton's indentation level.
   * 
   * @return size_t& reference
   */
  static size_t& indent()  { return instance().m_indent; }

  /**
   * @brief Get the indentation level.
   * 
   * @return size_t value
   */
  static size_t get_indent()  { return instance().m_indent; }

  /**
   * @brief Set the indentation level.
   * 
   * @param indent size_t
   */
  static void set_indent(size_t indent)  { instance().m_indent = indent; }

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
  static void reset_indent()  { instance().m_indent = 0; }



  /**
   * @brief Helper function to log an SDL error message for a specific
   *        subsystem initialization failure.
   * 
   * @param subsystem The name of the subsystem that failed to initialize.
   * 
   * @return SDL_AppResult SDL_APP_FAILURE for convenience.
   */
  //template <typename... Args>
  static inline SDL_AppResult error_init(std::string subsystem)
  {
    error("Failed to initialize {}: {}", subsystem, SDL_GetError());
    return SDL_APP_FAILURE;
  }

  /**
   * @brief Enables or disables info logging.
   */
  static inline constexpr void info_enable(bool enable = true)
  { log_info_enabled = enable; }

  static inline constexpr void info_disable()
  { info_enable(false); }



  /**
   * @brief Log a TRACE-level message, with optional indentation level.
   * 
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global indentation level.
   * @param message A printf-style format string for the log message.
   * @param args Format arguments for the log message.
   */
  template <typename... Args>
  static inline void trace(int indent, std::string message, Args&&... args)
  {
    SDL_LogIndent(indent, SDL_LogPriority::SDL_LOG_PRIORITY_TRACE,
                  message.c_str(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  static inline void trace(std::string message, Args&&... args)
  {
    trace(instance().indent(), message, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a VERBOSE-level message, with optional indentation level.
   * 
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global indentation level.
   * @param message A printf-style format string for the log message.
   * @param args Format arguments for the log message.
   */
  template <typename... Args>
  static inline void verbose(int indent, std::string message, Args&&... args)
  {
    SDL_LogIndent(indent, SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE,
                  message.c_str(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  static inline void verbose(std::string message, Args&&... args)
  {
    verbose(instance().indent(), message, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a DEBUG-level message, with optional indentation level.
   * 
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global indentation level.
   * @param message A printf-style format string for the log message.
   * @param args Format arguments for the log message.
   */
  template <typename... Args>
  static inline void debug(int indent, std::string message, Args&&... args)
  {
    SDL_LogIndent(indent, SDL_LogPriority::SDL_LOG_PRIORITY_DEBUG,
                  message.c_str(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  static inline void debug(std::string message, Args&&... args)
  {
    debug(instance().indent(), message, std::forward<Args>(args)...);
  }

  /**
   * @brief Log an INFO-level message, with optional indentation level.
   * 
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global indentation level.
   * @param message A printf-style format string for the log message.
   * @param args Format arguments for the log message.
   */
  template <typename... Args>
  static inline void info(int indent, std::string message, Args&&... args)
  {
    if (log_info_enabled)
    {
      SDL_LogIndent(indent, SDL_LogPriority::SDL_LOG_PRIORITY_INFO,
                    message.c_str(), std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  static inline void info(std::string message, Args&&... args)
  {
    info(instance().indent(), message, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a WARN-level message, with optional indentation level.
   * 
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global indentation level.
   * @param message A printf-style format string for the log message.
   * @param args Format arguments for the log message.
   * 
   * @return SDL_AppResult `SDL_APP_FAILURE` for convenience in error handling.
   */
  template <typename... Args>
  static inline SDL_AppResult warn(int indent, std::string message, Args&&... args)
  {
    SDL_LogIndent(indent, SDL_LogPriority::SDL_LOG_PRIORITY_WARN,
                  message.c_str(), std::forward<Args>(args)...);
    return SDL_APP_FAILURE;
  }

  template <typename... Args>
  static inline SDL_AppResult warn(std::string message, Args&&... args)
  {
    return warn(instance().indent(), message, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a ERROR-level message, with optional indentation level.
   * 
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global indentation level.
   * @param message A printf-style format string for the log message.
   * @param args Format arguments for the log message.
   * 
   * @return SDL_AppResult `SDL_APP_FAILURE` for convenience in error handling.
   */
  template <typename... Args>
  static inline SDL_AppResult error(int indent, std::string message, Args&&... args)
  {
    SDL_LogIndent(indent, SDL_LogPriority::SDL_LOG_PRIORITY_ERROR,
                  message.c_str(), std::forward<Args>(args)...);
    return SDL_APP_FAILURE;
  }

  template <typename... Args>
  static inline SDL_AppResult error(std::string message, Args&&... args)
  {
    return error(instance().indent(), message, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a CRITICAL-level message, with optional indentation level.
   * 
   * @tparam Args Variadic template parameters for the format arguments.
   * @param indent Number of indentation levels to apply.
   *               Omit this to call the overload that uses the global indentation level.
   * @param message A printf-style format string for the log message.
   * @param args Format arguments for the log message.
   * 
   * @return SDL_AppResult `SDL_APP_FAILURE` for convenience in error handling.
   */
  template <typename... Args>
  static inline SDL_AppResult critical(int indent, std::string message, Args&&... args)
  {
    SDL_LogIndent(indent, SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL,
                  message.c_str(), std::forward<Args>(args)...);
    return SDL_APP_FAILURE;
  }

  template <typename... Args>
  static inline SDL_AppResult critical(std::string message, Args&&... args)
  {
    return critical(instance().indent(), message, std::forward<Args>(args)...);
  }


  static inline constexpr std::string get_log_priority_name(SDL_LogPriority priority)
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


private:
  // Prevent instantiation from outside the class
  Log(): m_indent{0}  { }

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
  static inline void SDL_LogIndent(size_t indent, SDL_LogPriority priority,
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

};


} // namespace bipsy::sdl3_utils


#endif // BIPSY_SDL3_UTILS_HPP