#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_HPP
#define BIPSY_GAIASIM_SCREEN_HPP

#include "SDL3/SDL.h"

namespace bipsy
{

namespace gaiasim
{

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
public:
  Screen();
  ~Screen();

  virtual SDL_AppResult init() = 0;

  virtual SDL_AppResult event(SDL_Event *event) = 0;
  virtual SDL_AppResult update() = 0;
  virtual SDL_AppResult render() = 0;

  virtual void deinit() = 0;

}; // class Screen

} // namespace bipsy::gaiasim

} // namespace bipsy

#endif // BIPSY_GAIASIM_SCREEN_HPP