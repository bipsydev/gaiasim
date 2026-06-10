/**
 * @file ScreenMainGUI.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief The GUI system of ScreenMain.
 *
 * This gives `GameWorld::render` a 2D bounds to draw their final
 * texture buffer to, that is drawn to the screen within the
 * GUI system during `ScreenMainGUI::render`.
 * Interally, we use a set of `Panel`s to align the GUI into a gridbox of sorts,
 * then dynamically stretch them to the screen size.
 * Currently just rect primitives with a texture rendered from GameWorld.
 *
 * @version a_0.0.1-pre
 * @date 2026-05-23
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
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP


#include "Game.hpp"

#include "SDL3/SDL.h"  // IWYU pragma: keep SDL_FRect, SDL_Color, SDL_Texture...

#include <string>


namespace bipsy::gaiasim
{

// forward declare GameWorld
class GameWorld;

namespace gui
{

class ScreenMainGUI
{
  struct Panel
  {
    // values [0, 1]. This is the percentage of the width/height
    // of the full window that this panel takes up.
    SDL_FRect m_partition;

    // display/id title
    std::string m_title;

    // rendering color
    SDL_Color m_color;

    // Texture for title text
    SDL_Texture * m_title_texture  = nullptr;

    // Cached dimensions of the title texture when rendered with the larger
    // font.
    SDL_FRect m_title_texture_size = {0, 0, 0, 0};

    // Flag for if small font should be used
    bool m_use_small_font          = false;
  };

  // The GUI panels
  Panel m_main_panel;
  Panel m_left_sidebar;
  Panel m_top_sidebar;

  // Our cached window dimensions
  int m_window_width;
  int m_window_height;


public:
  ScreenMainGUI();
  ~ScreenMainGUI();

  SDL_AppResult update_layout(Game * game);
  SDL_AppResult render(SDL_Renderer * renderer, GameWorld * world);
  SDL_AppResult event(Game * game, SDL_Event * event);


private:
  SDL_AppResult generate_textures(Game * game);

  SDL_AppResult create_panel_title_texture(Game * game, Panel & panel);

  SDL_AppResult render_panel(SDL_Renderer * renderer,
                             Panel &        panel,
                             GameWorld *    world = nullptr);

};  // class ScreenMainGUI

}  // namespace gui

}  // namespace bipsy::gaiasim

#endif  // BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP
