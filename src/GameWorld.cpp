#include "GameWorld.hpp"

#include "Game.hpp"

#include "SDL_utils.hpp"

#include "SDL3_ttf/SDL_ttf.h"


namespace bipsy::gaiasim
{

using namespace sdlutils; // log_error_init, log_info, etc.


GameWorld::GameWorld(Game *game)
: m_game{game},
  map_texture{nullptr}
{
  // Initialize game world state here, using the Game instance if needed
}

SDL_AppResult GameWorld::init()
{
  // Generate the map texture based on the ASCII map
  return generate_map_texture();
}

GameWorld::~GameWorld()
{
  // Clean up game world resources here

  // Clean up map texture
  if (map_texture != nullptr)
  {
    SDL_DestroyTexture(map_texture);
    map_texture = nullptr;
  }
}

SDL_AppResult GameWorld::event(SDL_Event *event)
{
  // Handle game world events here (e.g. player input, NPC behavior, etc.)
  // For now, we don't have any interactive elements in the game world, so we just return CONTINUE.
  return SDL_APP_CONTINUE;
}

SDL_AppResult GameWorld::update()
{
  // Update game world state here (e.g. move NPCs, check for collisions, etc.)
  // For now, we don't have any dynamic elements in the game world, so we just return CONTINUE.
  return SDL_APP_CONTINUE;
}

SDL_AppResult GameWorld::render(SDL_Renderer *renderer, SDL_FRect *bounds)
{
  // Attempt to render map_texture if we have one
  if (map_texture != nullptr)
  {
    SDL_FRect map_rect = *bounds; // Render map to fill entire panel
    SDL_GetTextureSize(map_texture, &map_rect.w, &map_rect.h); // Get the width and height of the texture
    if (map_rect.w > bounds->w || map_rect.h > bounds->h)
    {
      // If the texture is larger than the panel, scale it down to fit while maintaining aspect ratio
      float width_ratio = bounds->w / map_rect.w;
      float height_ratio = bounds->h / map_rect.h;
      float scale = std::min(width_ratio, height_ratio);
      map_rect.w *= scale;
      map_rect.h *= scale;
    }
    map_rect.x = bounds->x + (bounds->w/2.0f) - (map_rect.w/2.0f); // Center map horizontally in panel
    map_rect.y = bounds->y + (bounds->h/2.0f) - (map_rect.h/2.0f); // Center map vertically in panel
    if (not SDL_RenderTexture(renderer, map_texture, NULL, &map_rect))
    {
      return log_error("Failed to render map texture: %s", 0, SDL_GetError());
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult GameWorld::generate_map_texture()
{
  SDL_Surface *map_surface = TTF_RenderText_Blended_Wrapped(m_game->font(), map.c_str(), 0, {255, 255, 255, 255}, 0);
  if(map_surface == nullptr)
  {
    return log_error_init("map_surface");
  }
  map_texture = SDL_CreateTextureFromSurface(m_game->renderer(), map_surface);
  SDL_DestroySurface(map_surface);
  if(map_texture == nullptr)
  {
    return log_error_init("map_texture");
  }

  return SDL_APP_CONTINUE;
}


} // namespace bipsy::gaiasim
