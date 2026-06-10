/**
 * @file Screen.cpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Implementation for `Screen` class methods

 * @version a_0.0.1-pre
 * @date 2026-05-24
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
#include "Screen.hpp"


#include "Game.hpp"  // Implementation from our forward declaration


namespace bipsy::gaiasim
{


SDL_AppResult Screen::show()
{
  // Set the game's clear color to this screen's clear color when shown
  game()->set_clear_color(m_clear_color);
  return SDL_APP_CONTINUE;
}


}  // namespace bipsy::gaiasim
