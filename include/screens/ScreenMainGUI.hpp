#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP

#include "SDL_utils.hpp"

#include "SDL3/SDL.h"

#include <string>


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
  : m_main_panel({0.25, 0.25, 0.75, 0.75}, "Main", {218, 255, 0, 255}),
    m_left_sidebar({0, 0.25, 0.25, 0.75}, "Left Sidebar", {255, 0, 203, 255}),
    m_top_sidebar({0, 0, 1.0, 0.25}, "Top Sidebar", {0, 180, 93, 255}),
    m_window_width{0}, m_window_height{0}
  {
  }
  ~ScreenMainGUI() { };

  SDL_AppResult update_layout(SDL_Renderer *renderer)
  {
    log_trace("ScreenMainGUI::update_layout called.");

    int window_width, window_height;
    if (not SDL_GetCurrentRenderOutputSize(
      renderer, &window_width, &window_height))
    {
      // Handle error
      return log_error("Failed to get current render output size: %s",
        SDL_GetError());
    }
    
    m_window_width = window_width;
    m_window_height = window_height;

    log_debug("Updated layout with window size %dx%d", 1, m_window_width, m_window_height);
    
    return SDL_APP_CONTINUE;
  }

  SDL_AppResult render(SDL_Renderer *renderer)
  {
    SDL_AppResult result;

    if (result = render_panel(renderer, m_left_sidebar))
      return result;

    if (result = render_panel(renderer, m_top_sidebar))
      return result;
      
    if (result = render_panel(renderer, m_main_panel))
      return result;

    return SDL_APP_CONTINUE;
  }


private:

  SDL_AppResult render_panel(SDL_Renderer *renderer, const Panel &panel)
  {
    // Set draw color to panel color and render filled rect
    SDL_SetRenderDrawColor(renderer,
        panel.m_color.r, panel.m_color.g, panel.m_color.b, panel.m_color.a);
    
    // Convert panel partition to actual pixel bounds
    SDL_FRect m_bounds;
    // multiply partition values by window dimensions to get pixel bounds
    m_bounds.x = panel.m_partition.x * m_window_width;
    m_bounds.y = panel.m_partition.y * m_window_height;
    m_bounds.w = panel.m_partition.w * m_window_width;
    m_bounds.h = panel.m_partition.h * m_window_height;
    if (not SDL_RenderFillRect(renderer, &m_bounds))
    {
      return log_error("Failed to render panel '%s': %s",
        panel.m_title.c_str(), SDL_GetError());
    }
    else
    {
      log_debug("Rendered panel '%s'", panel.m_title.c_str());
    }

    return SDL_APP_CONTINUE;
  }

}; // class ScreenMainGUI

} // namespace bipsy::gaiasim::gui

#endif // BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP