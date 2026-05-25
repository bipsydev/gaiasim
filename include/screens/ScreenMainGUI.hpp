#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP

#include "SDL_utils.hpp"

#include "SDL3/SDL.h"

#include <string>
#include <format>


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
  int window_width;
  int window_height;


public:
  ScreenMainGUI()
  : m_main_panel({0.25, 0.25, 0.75, 0.75}, "Main", {218, 255, 0, 255}),
    m_left_sidebar({0, 0.25, 0.25, 0.75}, "Left Sidebar", {255, 0, 203, 255}),
    m_top_sidebar({0, 0, 1.0, 0.25}, "Top Sidebar", {0, 180, 93, 255}),
    window_width{0}, window_height{0}
  {
  }
  ~ScreenMainGUI() { };

  void render(SDL_Renderer *renderer)
  {
    int window_width, window_height;
    if (not SDL_GetCurrentRenderOutputSize(
      renderer, &window_width, &window_height))
    {
      // Handle error
      log_error(std::format(
        "Failed to get current render output size: {}", SDL_GetError()));
      return;
    }
    // continue rendering with window dimensions
    update_layout(window_width, window_height);

    render_panel(renderer, m_left_sidebar);
    render_panel(renderer, m_top_sidebar);
    render_panel(renderer, m_main_panel);
  }

private:
  void update_layout(int window_width, int window_height)
  {
    log_info(std::format(
      "ScreenMainGUI::update_layout({}, {}) called.",
      window_width, window_height));
    this->window_width = window_width;
    this->window_height = window_height;
  }

  SDL_AppResult render_panel(SDL_Renderer *renderer, const Panel &panel)
  {
    // Set draw color to panel color and render filled rect
    SDL_SetRenderDrawColor(renderer,
        panel.m_color.r, panel.m_color.g, panel.m_color.b, panel.m_color.a);
    
    // Convert panel partition to actual pixel bounds
    SDL_FRect m_bounds;
    // multiply partition values by window dimensions to get pixel bounds
    m_bounds.x = panel.m_partition.x * window_width;
    m_bounds.y = panel.m_partition.y * window_height;
    m_bounds.w = panel.m_partition.w * window_width;
    m_bounds.h = panel.m_partition.h * window_height;
    if (not SDL_RenderFillRect(renderer, &m_bounds))
    {
      return log_error(std::format(
        "Failed to render panel '{}': {}", panel.m_title, SDL_GetError()));
    }
    else
    {
      log_info(std::format("Rendered panel '{}'", panel.m_title));
    }

    return SDL_APP_SUCCESS;
  }
};

} // namespace bipsy::gaiasim::gui

#endif // BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP