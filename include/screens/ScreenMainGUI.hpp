#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP

#include "SDL_utils.hpp"
#include "Game.hpp"

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <string>
#include <algorithm> // std::max, std::min


namespace bipsy::gaiasim
{
// forward declare GameWorld
class GameWorld;



namespace gui
{

using namespace bipsy::sdlutils;  // log_error, etc


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
    SDL_Texture *m_title_texture = nullptr;

    // Cached dimensions of the title texture when rendered with the larger font.
    SDL_FRect m_title_texture_size = {0, 0, 0, 0};

    // Flag for if small font should be used
    bool m_use_small_font = false;
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

  SDL_AppResult update_layout(Game *game);
  SDL_AppResult render(SDL_Renderer *renderer, GameWorld *world);


private:

  SDL_AppResult generate_textures(Game *game);

  SDL_AppResult create_panel_title_texture(Game *game, Panel &panel);

  SDL_AppResult render_panel(SDL_Renderer *renderer, Panel &panel,
                             GameWorld *world = nullptr);

}; // class ScreenMainGUI

} // namespace bipsy::gaiasim::gui

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP