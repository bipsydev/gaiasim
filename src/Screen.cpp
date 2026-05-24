#include "Screen.hpp"

#include "Game.hpp" // for `game()` getter, to complete the `Game` type

namespace bipsy::gaiasim
{

  SDL_AppResult Screen::show()
  {
    // Set the game's clear color to this screen's clear color when shown
    game()->set_clear_color(clear_color);
    return SDL_APP_CONTINUE;
  }

} // namespace bipsy::gaiasim