#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP

#include "SDL_utils.hpp"
#include "Game.hpp"

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <string>
#include <algorithm> // std::max, std::min


namespace bipsy::gaiasim::gui
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

  SDL_AppResult render(SDL_Renderer *renderer, SDL_Texture *map_texture)
  {
    SDL_AppResult result;

    if (result = render_panel(renderer, m_left_sidebar))
      return result;

    if (result = render_panel(renderer, m_top_sidebar))
      return result;
      
    if (result = render_panel(renderer, m_main_panel, map_texture))
      return result;

    return SDL_APP_CONTINUE;
  }


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

  SDL_AppResult create_panel_title_texture(Game *game, Panel &panel)
  {
    TTF_Font *panel_font = panel.m_use_small_font ?
        game->font_small() :
#ifndef __ANDROID__
        game->font();
#else
        game->font_hidpi();
#endif
    // Determine title text color
    SDL_Color text_color = {0, 0, 0, 255}; // default to black
    
    Uint8 brightness = (panel.m_color.r * 0.2126f) + (panel.m_color.g * 0.7152f) + (panel.m_color.b * 0.0722f);
    if (brightness < 128)
    {
      text_color = {255, 255, 255, 255}; // use white 
    }

    SDL_Surface * text_surface = TTF_RenderText_Blended(
      panel_font, panel.m_title.c_str(),
      0, text_color
    );
    if (text_surface == nullptr)
    {
      return log_error_init("text_surface");
    }

    // Create a texture from the surface
    // This uploads the image data to the GPU for efficient rendering.
    panel.m_title_texture = SDL_CreateTextureFromSurface(game->renderer(), text_surface);
    SDL_DestroySurface(text_surface); // We can free the surface after creating the texture
    if (panel.m_title_texture == nullptr)
    {
      return log_error_init("panel.m_title_texture");
    }

    // Cache larger font texture dimensions for later layout calculations
    if (panel.m_title_texture_size.w == 0 && panel.m_title_texture_size.h == 0
      && !panel.m_use_small_font)
    {
      SDL_GetTextureSize(panel.m_title_texture,
        &panel.m_title_texture_size.w, &panel.m_title_texture_size.h);
    }
    
    return SDL_APP_CONTINUE;
  }

  SDL_AppResult render_panel(SDL_Renderer *renderer, Panel &panel, SDL_Texture *map_texture = nullptr)
  {
    // Set draw color to panel color and render filled rect
    if (not SDL_SetRenderDrawColor(renderer,
        panel.m_color.r, panel.m_color.g, panel.m_color.b, panel.m_color.a))
    {
      return log_error("Failed to set panel '%s' render draw color: %s",
        panel.m_title.c_str(), SDL_GetError());
    }

    // Convert panel partition to actual pixel bounds
    SDL_FRect m_bounds;
    // multiply partition values by window dimensions to get pixel bounds
    m_bounds.x = panel.m_partition.x * m_window_width;
    m_bounds.y = panel.m_partition.y * m_window_height;
    m_bounds.w = panel.m_partition.w * m_window_width;
    m_bounds.h = panel.m_partition.h * m_window_height;

    // attempt to draw the rectangle
    if (not SDL_RenderFillRect(renderer, &m_bounds))
    {
      return log_error("Failed to render panel '%s': %s",
        panel.m_title.c_str(), SDL_GetError());
    }
    else
      log_debug("Rendered panel '%s'", panel.m_title.c_str());

    // Render the title texture for this panel, if it exists
    if (panel.m_title_texture != nullptr)
    {
      // position to render text - we will render the title in the top left corner of the
      // panel with some padding
      const float padding_x = 5.0f;
      const float padding_y = 0.0f;
      SDL_FRect text_rect;

      SDL_GetTextureSize(panel.m_title_texture, &text_rect.w, &text_rect.h); // Get the width and height of the texture
      // We want the entire texture, unless clipping
      SDL_FRect source_rect = {0, 0, text_rect.w, text_rect.h};
      
      // use width & height for centering and clipping if necessary
      text_rect.x = std::min(std::max(
        m_bounds.x + padding_x,
          m_bounds.x + (m_bounds.w/2.0f) - (text_rect.w/2.0f)),
        m_bounds.x + m_bounds.w - padding_x);
      
      text_rect.y = std::min(std::max(
        m_bounds.y + padding_y,
          m_bounds.y),
        m_bounds.y + m_bounds.h - padding_y);

      // clip text rect to panel bounds if necessary
      if (text_rect.x + text_rect.w > m_bounds.x + m_bounds.w)
      {
        float diff = (text_rect.x + text_rect.w) - (m_bounds.x + m_bounds.w);
        text_rect.w -= diff;
        source_rect.w -= diff;
        panel.m_use_small_font = true;
      }
      if (text_rect.y + text_rect.h > m_bounds.y + m_bounds.h)
      {
        float diff = (text_rect.y + text_rect.h) - (m_bounds.y + m_bounds.h);
        text_rect.h -= diff;
        source_rect.h -= diff;
        panel.m_use_small_font = true;
      }
      // determine if we should switch back to larger font
      if (panel.m_use_small_font
        && (m_bounds.w > panel.m_title_texture_size.w + 2*padding_x
          && m_bounds.h > panel.m_title_texture_size.h + 2*padding_y))
      {
        panel.m_use_small_font = false;
      }

      // Attempt to render map_texture if we have one
      if (map_texture != nullptr)
      {
        SDL_FRect map_rect = m_bounds; // Render map to fill entire panel
        SDL_GetTextureSize(map_texture, &map_rect.w, &map_rect.h); // Get the width and height of the texture
        if (map_rect.w > m_bounds.w || map_rect.h > m_bounds.h)
        {
          // If the texture is larger than the panel, scale it down to fit while maintaining aspect ratio
          float width_ratio = m_bounds.w / map_rect.w;
          float height_ratio = m_bounds.h / map_rect.h;
          float scale = std::min(width_ratio, height_ratio);
          map_rect.w *= scale;
          map_rect.h *= scale;
        }
        map_rect.x = m_bounds.x + (m_bounds.w/2.0f) - (map_rect.w/2.0f); // Center map horizontally in panel
        map_rect.y = m_bounds.y + (m_bounds.h/2.0f) - (map_rect.h/2.0f); // Center map vertically in panel
        if (not SDL_RenderTexture(renderer, map_texture, NULL, &map_rect))
        {
          return log_error("Failed to render map texture for panel '%s': %s",
            panel.m_title.c_str(), SDL_GetError());
        }
      }

      // Attempt to render the title texture
      if (not SDL_RenderTexture(renderer, panel.m_title_texture, &source_rect, &text_rect))
      {
        return log_error("Failed to render title texture for panel '%s': %s",
          panel.m_title.c_str(), SDL_GetError());
      }
    }

    return SDL_APP_CONTINUE;
  }

}; // class ScreenMainGUI

} // namespace bipsy::gaiasim::gui

#endif // BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP