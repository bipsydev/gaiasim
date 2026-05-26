#include "screens/ScreenMainGUI.hpp"

// full definition of the class we forward declared
#include "GameWorld.hpp"

#include "SDL_utils.hpp"


namespace bipsy::gaiasim::gui
{

using namespace sdlutils;  // log_error, log_warn, etc


SDL_AppResult ScreenMainGUI::render(SDL_Renderer *renderer, GameWorld *world)
{
  SDL_AppResult result;

  if (result = render_panel(renderer, m_left_sidebar))
    return result;

  if (result = render_panel(renderer, m_top_sidebar))
    return result;
    
  if (result = render_panel(renderer, m_main_panel, world))
    return result;

  return SDL_APP_CONTINUE;
}




SDL_AppResult ScreenMainGUI::create_panel_title_texture(Game *game, Panel &panel)
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


SDL_AppResult ScreenMainGUI::render_panel(SDL_Renderer *renderer, Panel &panel, GameWorld *world)
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

    // RENDER THE GAME WORLD here!
    if (world != nullptr) // if this panel contains a game world pointer...
    {
      // attempt to render, and if it fails, log error
      SDL_AppResult result;
      if ( (result = (world->render(renderer, &m_bounds))) )
      {
        log_error("Failed to render map texture for panel '%s': %s", 0,
          panel.m_title.c_str(), SDL_GetError());
        return result;
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



} // namespace bipsy::gaiasim::gui
