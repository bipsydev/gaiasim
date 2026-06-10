#include "screens/ScreenMainGUI.hpp"


#include "GameWorld.hpp"  // Implementation from our forward declaration
#include "SDL3_utils.hpp"

#include "SDL3/SDL.h"// IWYU pragma: keep SDL_DestroyTexture, SDL_Renderer...


namespace bipsy::gaiasim::gui
{

using bipsy::sdl3_utils::Log;



ScreenMainGUI::ScreenMainGUI()
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



ScreenMainGUI::~ScreenMainGUI()
{
  // Clean up textures
  if (m_main_panel.m_title_texture != nullptr)
    SDL_DestroyTexture(m_main_panel.m_title_texture);
  if (m_left_sidebar.m_title_texture != nullptr)
    SDL_DestroyTexture(m_left_sidebar.m_title_texture);
  if (m_top_sidebar.m_title_texture != nullptr)
    SDL_DestroyTexture(m_top_sidebar.m_title_texture);
}



SDL_AppResult ScreenMainGUI::update_layout(Game *game)
{
  LOG_FRAME_CLASS(ScreenMainGUI);

  int window_width, window_height;
  if (not SDL_GetCurrentRenderOutputSize(
    game->renderer(), &window_width, &window_height))
  {
    // Handle error
    return Log::error("Failed to get current render output size: {}", SDL_GetError());
  }
  
  bool changed = false;
  // check if window size has changed since last update
  if (window_width != m_window_width || window_height != m_window_height)
    changed = true;

  m_window_width = window_width;
  m_window_height = window_height;

  Log::verbose(Log::indent() + 1, "Updated layout with window size {}x{}", m_window_width, m_window_height);
  
  // regenerate textures based on window size change
  if (changed)
    return generate_textures(game);
  // continue otherwise
  return SDL_APP_CONTINUE;
}



SDL_AppResult ScreenMainGUI::render(SDL_Renderer *renderer, GameWorld *world)
{
  LOG_FRAME_CLASS(ScreenMainGUI);

  SDL_AppResult result;

  if (result = render_panel(renderer, m_left_sidebar))
    return result;

  if (result = render_panel(renderer, m_top_sidebar))
    return result;
    
  if (result = render_panel(renderer, m_main_panel, world))
    return result;

  return SDL_APP_CONTINUE;
}



SDL_AppResult ScreenMainGUI::event(Game *game, SDL_Event *event)
{
  // For now, we don't have any interactive elements in the GUI, so we just return CONTINUE.
  // In the future, this is where we would handle events like mouse clicks on buttons, etc.
  return SDL_APP_CONTINUE;
}



SDL_AppResult ScreenMainGUI::generate_textures(Game *game)
{
  LOG_FRAME_CLASS(ScreenMainGUI);

  Log::debug("Generating title textures for panels...");

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
    return Log::error("Failed to create title texture for main panel");
  if (create_panel_title_texture(game, m_left_sidebar))
    return Log::error("Failed to create title texture for left sidebar");
  if (create_panel_title_texture(game, m_top_sidebar))
    return Log::error("Failed to create title texture for top sidebar");
  
  return SDL_APP_CONTINUE;
}



SDL_AppResult ScreenMainGUI::create_panel_title_texture(Game *game, Panel &panel)
{
  LOG_FRAME_CLASS(ScreenMainGUI);

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
    return Log::error_init("text_surface");
  }

  // Create a texture from the surface
  // This uploads the image data to the GPU for efficient rendering.
  panel.m_title_texture = SDL_CreateTextureFromSurface(game->renderer(), text_surface);
  SDL_DestroySurface(text_surface); // We can free the surface after creating the texture
  if (panel.m_title_texture == nullptr)
  {
    return Log::error_init("panel.m_title_texture");
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



SDL_AppResult ScreenMainGUI::render_panel(SDL_Renderer *renderer, Panel &panel, GameWorld *world)
{
  LOG_FRAME_CLASS(ScreenMainGUI);

  // Set draw color to panel color and render filled rect
  if (not SDL_SetRenderDrawColor(renderer,
      panel.m_color.r, panel.m_color.g, panel.m_color.b, panel.m_color.a))
  {
    return Log::error("Failed to set panel '{}' render draw color: {}",
      panel.m_title, SDL_GetError());
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
    return Log::error("Failed to render panel '{}': {}",
      panel.m_title, SDL_GetError());
  }
  else
    Log::debug("Rendered panel '{}'", panel.m_title);

  // RENDER THE GAME WORLD here!
  if (world != nullptr) // if this panel contains a game world pointer...
  {
    // attempt to render, and if it fails, log error
    SDL_AppResult result;
    if ( (result = (world->render(renderer, &m_bounds))) )
    {
      Log::error("Failed to render map texture for panel '{}': {}",
        panel.m_title, SDL_GetError());
      return result;
    }
  }

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

    // Attempt to render the title texture
    if (not SDL_RenderTexture(renderer, panel.m_title_texture, &source_rect, &text_rect))
    {
      return Log::error("Failed to render title texture for panel '{}': {}",
        panel.m_title, SDL_GetError());
    }
  }

  return SDL_APP_CONTINUE;
}



} // namespace bipsy::gaiasim::gui
