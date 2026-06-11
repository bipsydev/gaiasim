/**
 * @file Screen.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Abstract base interface for a game "screen" (e.g. main menu, settings,
 *        gameplay, etc).
 *
 * Essentially just contains the shared logic between child screens
 * and contains the base interface for containing a list of abstract Screens.
 * Holds the screen's name, background clear color for rendering,
 * and a reference to the parent Game class that controls this Screen.
 *
 * @version a_0.0.1-pre
 * @date 2026-05-15
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
#ifndef BIPSY_GAIASIM_SCREEN_HPP
#define BIPSY_GAIASIM_SCREEN_HPP


// #region Library Includes

#include "biputils/SDL3.hpp"

#include "SDL3/SDL.h"  // IWYU pragma: keep SDL_Color, SDL_AppResult

#include <string>


// #endregion

namespace bipsy::gaiasim
{

// #region Forward Declarations

class Game;

// #endregion

/**
 * @brief Abstract base interface for a game screen.
 *
 * This takes over the `Game`'s main callbacks (event, iterate, etc)
 * when active, and is responsible for handling those callbacks
 * (updating screen logic and rendering the screen).
 * Subclasses of this interface will be used for different screens in the game
 * (main menu, settings, gameplay, loading, etc).
 */
class Screen
{

  // #region Private Data Members

  // Parent Game instance that owns this Screen.
  Game * m_game_ptr;
  // Name of the screen, for display purposes.
  std::string m_name_str;

  // Clear color for this screen
  SDL_Color m_clear_color;


  // #endregion

public:

  // #region Constructors and Destructor

  Screen(Game *              game,
         const std::string & name,
         SDL_Color           color = sdl3_utils::GAME_CLEAR_COLOR_DEFAULT)
  : m_game_ptr(game), m_name_str(name), m_clear_color(color)
  {
    LOG_FRAME_CLASS(Screen);
    bipsy::sdl3_utils::Log::verbose(
            "Constructed screen \"{}\" with bg clear color RGBA({}, "
            "{}, {}, {})",
            m_name_str,
            m_clear_color.r,
            m_clear_color.g,
            m_clear_color.b,
            m_clear_color.a
    );
  }

  // Prevent copying (for now) because of pointer data members
  // May implement later if the need arises
  Screen(const Screen &)              = delete;
  Screen & operator =(const Screen &) = delete;

  virtual ~Screen() { m_game_ptr = nullptr; }


  // #endregion
  // #region Getters

  constexpr Game * const        game() const { return m_game_ptr; }

  constexpr const std::string & name() const { return m_name_str; }


  // #endregion
  // #region Virtual Functions: Abstract Lifecycle API for child classes
  /**
   * @brief Initialize local Screen data and state.
   *
   * This is called when the Screen is allocated. It is called only once
   * per instantiation. Any code that is meant to run when the Screen is
   * shown (but not allocated) should go in the `show()` function instead.
   * When the Screen is allocated it is considered active, even if not currently
   * shown on the screen. It's still allocated, and could still even be running
   * and updating/catching events in the background.
   *
   * @return SDL_AppResult SDL_APP_CONTINUE if initialization succeeded
   *                                        and the app should continue,
   *                       SDL_APP_FAILURE if initialization failed and
   *                                       the app should terminate with
   *                                       an error.
   */
  virtual SDL_AppResult init()                          = 0;

  virtual SDL_AppResult event(SDL_Event * event)        = 0;
  virtual SDL_AppResult update()                        = 0;
  virtual SDL_AppResult render(SDL_Renderer * renderer) = 0;

  virtual SDL_AppResult post_render_update() { return SDL_APP_CONTINUE; }

  virtual SDL_AppResult show();

  virtual SDL_AppResult hide() { return SDL_APP_CONTINUE; }

  // #endregion

};  // class Screen

}  // namespace bipsy::gaiasim

#endif  // BIPSY_GAIASIM_SCREEN_HPP
