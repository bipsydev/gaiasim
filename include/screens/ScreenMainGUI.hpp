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
  ScreenMainGUI()
  : m_main_panel(
        {0.25, 0.25, 0.75, 0.75},
        "Main",
        {0x00, 0x00, 0x00, 0xFF}),
    m_left_sidebar(
        {0, 0.25, 0.25, 0.75},
        "Left Sidebar",
        {0xFF, 0x00, 0xCB, 0xFF}),
    m_top_sidebar(
        {0, 0, 1.0, 0.25},
        "Top Sidebar",
        {0x00, 0xB4, 0x5D, 0xFF}),
    m_window_width{0}, m_window_height{0}
  { }

  ~ScreenMainGUI()
  {
    // Clean up textures
    if (m_main_panel.m_title_texture != nullptr)
      SDL_DestroyTexture(m_main_panel.m_title_texture);
    if (m_left_sidebar.m_title_texture != nullptr)
      SDL_DestroyTexture(m_left_sidebar.m_title_texture);
    if (m_top_sidebar.m_title_texture != nullptr)
      SDL_DestroyTexture(m_top_sidebar.m_title_texture);
  }

  SDL_AppResult update_layout(Game *game)
  {
    log_trace("ScreenMainGUI::update_layout called.");

    int window_width, window_height;
    if (not SDL_GetCurrentRenderOutputSize(
      game->renderer(), &window_width, &window_height))
    {
      // Handle error
      return log_error("Failed to get current render output size: %s",
        SDL_GetError());
    }
    
    bool changed = false;
    // check if window size has changed since last update
    if (window_width != m_window_width || window_height != m_window_height)
      changed = true;

    m_window_width = window_width;
    m_window_height = window_height;

    log_debug("Updated layout with window size %dx%d", 1, m_window_width, m_window_height);
    
    // regenerate textures based on window size change
    if (changed)
      return generate_textures(game);
    // continue otherwise
    return SDL_APP_CONTINUE;
  }

  SDL_AppResult render(SDL_Renderer *renderer, GameWorld *world);


private:

  SDL_AppResult generate_textures(Game *game)
  {
    log_debug("Generating title textures for panels...");

    // free anything already allocated
    if (m_main_panel.m_title_texture != nullptr)
    {
      SDL_DestroyTexture(m_main_panel.m_title_texture);
      m_main_panel.m_title_texture = nullptr;
    }
    if (m_left_sidebar.m_title_texture != nullptr)
    {
      SDL_DestroyTexture(m_left_sidebar.m_title_texture);
      m_left_sidebar.m_title_texture = nullptr;
    }
    if (m_top_sidebar.m_title_texture != nullptr)
    {
      SDL_DestroyTexture(m_top_sidebar.m_title_texture);
      m_top_sidebar.m_title_texture = nullptr;
    }

    // allocate all 3
    if (create_panel_title_texture(game, m_main_panel))
      return log_error("Failed to create title texture for main panel");
    if (create_panel_title_texture(game, m_left_sidebar))
      return log_error("Failed to create title texture for left sidebar");
    if (create_panel_title_texture(game, m_top_sidebar))
      return log_error("Failed to create title texture for top sidebar");
    
    return SDL_APP_CONTINUE;
  }

  SDL_AppResult create_panel_title_texture(Game *game, Panel &panel);

  SDL_AppResult render_panel(SDL_Renderer *renderer, Panel &panel,
                             GameWorld *world = nullptr);

}; // class ScreenMainGUI

} // namespace bipsy::gaiasim::gui

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP