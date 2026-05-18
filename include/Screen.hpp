#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_HPP
#define BIPSY_GAIASIM_SCREEN_HPP

#include "SDL3/SDL.h"

#include <string>


namespace bipsy::gaiasim
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
  std::string name_str;

public:
  Screen(const std::string &name) : name_str(name)  { };
  virtual ~Screen()  { };

  const std::string &name() const
  {
    return name_str;
  }

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
   *                                       the app should terminate with an error.
   */
  virtual SDL_AppResult init() = 0;

  virtual SDL_AppResult event(SDL_Event *event) = 0;
  virtual SDL_AppResult update() = 0;
  virtual SDL_AppResult render(SDL_Renderer *renderer) = 0;
  virtual SDL_AppResult post_render_update()  { return SDL_APP_CONTINUE; }

  virtual SDL_AppResult show()  { return SDL_APP_CONTINUE; }
  virtual SDL_AppResult hide()  { return SDL_APP_CONTINUE; }

  virtual void deinit()  { };

}; // class Screen

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_SCREEN_HPP