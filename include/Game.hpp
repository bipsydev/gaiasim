/**
 * @file Game.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Adapter class for SDL3's main callbacks that manages `Screen`s
 *
 * This class encapsulates game data and provides functions to call
 * at each of SDL3's main callback entrypoints.
 * This is used as the `appstate` pointer for the entrypoint system
 * to hold global game state for the duration of the program's execution.
 * This class also manages a list of `Screen`s, which are game state screens
 * that may be either the active displayed screen, or running/paused.
 * Each Screen also implements functions for each SDL3 callback entrypoint,
 * but one or many Screens may be running each frame.
 *
 * @version a_0.0.1-pre
 * @date 2026-05-10
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
#ifndef BIPSY_GAIASIM_GAME_HPP
#define BIPSY_GAIASIM_GAME_HPP


#include "biputils/formatter.hpp"  // FORMATTER() specialization for std::format
#include "biputils/SDL3.hpp"
#include "Screen.hpp"

#include "SDL3/SDL.h"  // IWYU pragma: keep Uint8, SDL_Window, SDL_Renderer...
#include "SDL3_ttf/SDL_ttf.h"

#include <cassert>
#include <vector>
#include <concepts>  // std::derived_from


namespace bipsy::gaiasim
{


/**
 * @brief Local application running state structure.
 *
 * Contains pointers to SDL objects for window, renderer, etc along with
 * other local application running state information. This object is
 * initialized once in SDL_AppInit and then passed around in the rest of the
 * SDL callback entrypoints as a `void*` (must be cast into a `Appstate*`).
 */
class Game
{
  /****************
   * DATA MEMBERS *
   ****************/

  // -- Screen management --

  /// List of all allocated screens (may or may not be active)
  std::vector<Screen *> m_screens;

  /// Index of the currently active screen in `screens`
  Uint8 m_active_screen_index;

  // -- SDL objects --
  SDL_Window *   m_window;       // Window object
  SDL_Renderer * m_renderer;     // Rendering context to window

  SDL_Color      m_clear_color;  // Clear/bg color for the renderer

  TTF_Font *     m_font;         // Global font
  TTF_Font *     m_font_small;   // Smaller font
  TTF_Font *     m_font_large;   // Larger font
  // Alias for larger font, for HiDPI displays
  TTF_Font *& m_font_hidpi = m_font_large;

  // -- Time-related variables --
  int    m_frame;          // Frame count
  Uint64 m_time_ns;        // Time (nanoseconds) since SDL initialization
                           // (updated at the beginning of each frame)
  Uint64 m_delta_time_ns;  // Time (ns) taken to render the previous frame

public:
  /**
   * @brief Flags for requesting/indicating certain initialization steps.
   *
   * Each consecutive flag requires previous steps to also be initialized.
   * The order is as follows:
   *
   *        LIBRARIES -> SYSTEM_OBJECTS -> GAME_STATE
   *
   * So for example, if you request GAME_STATE initialization, it will
   * also initialize SYSTEM_OBJECTS and LIBRARIES in order before
   * initializing GAME_STATE.
   *
   * LIBRARIES:      SDL, it's subsystems, and any other external libraries.
   *
   * SYSTEM_OBJECTS: Global SDL/system objects like window, renderer, etc
   *                 that are allocated for the entire duration
   *                 of the `Game`'s lifetime.
   *
   * GAME_STATE:     The initial game state, which includes loading assets and
   *                 starting an initial beginning scene.
   */
  enum InitRequest : Uint8
  {
    NONE           = 0U,
    LIBRARIES      = 1U,
    SYSTEM_OBJECTS = 2U,
    GAME_STATE     = 3U,
    ALL            = 3U,  // Same as GAME_STATE
  };

private:
  InitRequest m_inits_complete;  // This tracks which initialization steps have
                                 // been completed

public:
  /******************************
   * STATIC METHOD DECLARATIONS *
   ******************************/

  static SDL_AppResult new_game(void *&     appstate,
                                InitRequest initializations = ALL);

  /***********************
   * METHOD DECLARATIONS *
   ***********************/

  /**
   * @brief Construct a new Game object.
   *
   * This also calls every `init_*` function in order by default.
   *
   * @param initializations Flags for requesting certain initialization
   *                        steps in the constructor.
   *
   */
  Game(InitRequest initializations = ALL);

  // Delete copy constructor to prevent copying (may implement later if needed)
  Game(const Game &)              = delete;
  // Delete copy assignment operator to prevent copying (may implement later)
  Game & operator =(const Game &) = delete;

  /**
   * @brief Request a specific initialization phase to complete to.
   *
   * @param initializations The initialization phase we're requesting to get to.
   * @return SDL_AppResult
   */
  SDL_AppResult init(InitRequest initializations = ALL);


  /**
   * @brief Initialize external libraries (SDL and subsystems)
   *
   * Potentially other libraries later, if needed.
   *
   * @return SDL_AppResult
   */
  SDL_AppResult init_libraries();

  /**
   * @brief Initialize system/library objects and global assets.
   *
   * This is the window, renderer, and other SDL system objects and
   * global fonts/textures.
   *
   * @return SDL_AppResult
   */
  SDL_AppResult init_system_objects();

  /**
   * @brief Initialize the game state by setting up the Screens and their data.
   *
   * Potentially should be re-callable from later to re-initialize the game
   * state if we ever we want to reset the game.
   *
   * @return SDL_AppResult
   */
  SDL_AppResult init_game_state();

  /**
   * @brief React to an SDL event by feeding it to the active screen.
   *
   * @param[in] event the input event from the SDL event entrypoint to process.
   * @return SDL_AppResult
   */
  SDL_AppResult event(SDL_Event * event);

  /**
   * @brief Splits SDL's frame iteration event into 3 organized phases.
   *
   * update -> render -> post_render_update
   *
   * @return SDL_AppResult
   */
  SDL_AppResult iterate();

  /**
   * @brief Update game logic before rendering the frame, passing to Screen.
   *
   * @return SDL_AppResult
   */
  SDL_AppResult update();

  /**
   * @brief Clears the game screen & sets up the renderer for the active Screen.
   *
   * @return SDL_AppResult
   */
  SDL_AppResult render();

  /**
   * @brief Update logic that is after rendering the frame, passed to Screen.
   *
   * @return SDL_AppResult
   */
  SDL_AppResult post_render_update();


  /**
   * @brief Getter for a pointer to the active Screen (displayed currently)
   *
   * @return constexpr Screen* The currently actively displayed screen.
   */
  constexpr Screen * active_screen() const
  {
    assert(!m_screens.empty() && "No screens available!");

    // use index to return active Screen
    return m_screens[m_active_screen_index];
  }

// --- Getters for data members ---

/**
 * @brief Helper macro to generate getters for data members
 * @details Each GETTER generates two functions:
 *          - one named 'name()' that returns `m_name`
 *          - and another named 'get_name()' that does the same thing.
 */
#define GETTER(type, name)                     \
  type name() const { return m_##name; }       \
  type get_##name() const { return m_##name; }

#define GETTER_CODE(type, name, code)                       \
  type name() const {code} type get_##name() const { code }

#define SETTER(type, name)                          \
  void set_##name(type value) { m_##name = value; }

  // Getters for time-related variables and FPS
  GETTER(Uint64, time_ns)
  GETTER(Uint64, delta_time_ns)

  GETTER_CODE(double,
              fps,  //
              {
                // Avoid division by zero, return 0 FPS if delta_time_ns is 0
                if (m_delta_time_ns == 0) return 0.0;
                return 1e9 / static_cast<double>(m_delta_time_ns);
              })

  GETTER(std::vector<Screen *>, screens)

  // Getters for SDL objects
  GETTER(SDL_Window * const, window)
  GETTER(SDL_Renderer * const, renderer)
  GETTER(TTF_Font * const, font)
  GETTER(TTF_Font * const, font_small)
  GETTER(TTF_Font * const, font_large)
  GETTER(TTF_Font * const, font_hidpi)

  GETTER(SDL_Color, clear_color)
  SETTER(SDL_Color, clear_color)


  /**
   * @brief Template function for adding a Screen of any subtype.
   *
   * @tparam ScreenType a type derived from `Screen`.
   * @tparam Args Variadic parameter pack of arguments
                  forwarded to the screen constructor.
   * @param args Variadic arguments forwarded to the screen constructor.
   * @return SDL_AppResult
   */
  template <std::derived_from<Screen> ScreenType, typename... Args>
  SDL_AppResult add_screen(Args &&... args)
  {
    using bipsy::sdl3_utils::Log;

    // Create a new screen instance with the provided arguments
    Screen * new_screen = new ScreenType(this, std::forward<Args>(args)...);

    // Initialize the new screen and check for errors
    if (SDL_AppResult result = new_screen->init())
    {
      Log::error(
              "Error occurred while initializing new screen, terminating..."
      );
      delete new_screen;  // Clean up allocated memory on failure
      return result;
    }

    // Add the new screen to the list of screens
    m_screens.push_back(new_screen);
    Log::info("Added new screen: {}", new_screen->name());

    return SDL_APP_CONTINUE;
  }

  /**
   * @brief Switch the active screen to the given screen index integer.
   *
   * @param screen_index integer into the vector of screens.
   * @return true if the switch
   * @return false
   */
  bool switch_screen(Uint8 screen_index);

  /**
   * @brief Deinitialize/free based on saved initializeation phase state.
   */
  ~Game();


};  // class Game

}  // namespace bipsy::gaiasim


// --- std::formatter specializations, defined in global namespace ---

// InitRequest
FORMATTER_ENUM(bipsy::gaiasim::Game::InitRequest);


#endif  // BIPSY_GAIASIM_GAME_HPP
