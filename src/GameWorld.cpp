#include "GameWorld.hpp"

#include "Game.hpp"
#include "SDL3_utils.hpp"

#include "SDL3_ttf/SDL_ttf.h"

#include <algorithm> // std::min, std::max


namespace bipsy::gaiasim
{

using bipsy::sdl3_utils::Log;


GameWorld::GameWorld(Game *game)
: m_game{game},
  world_map{},
  map_texture{nullptr}
{ }

SDL_AppResult GameWorld::init()
{
  SDL_AppResult result;

  // Generate the map texture based on the ASCII map
  if (result = generate_map_texture())
  {
    return result;
  }

  // generate the world map (chunks & blocks)
  return generate_world_map();
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
  // respond to arrow keys/WASD
  if (event->type == SDL_EVENT_KEY_DOWN)
  {
    // reset the flag that tracks whether the player moved this frame
    moved = false;
    // `render` will set this back to false when the flag is handled
    
    Uint8 new_x = player_x;
    Uint8 new_y = player_y;

#define NEW_POS(func, arg1, arg2) \
  static_cast<Uint8>(std::func<Sint16>(arg1, arg2))

    // set `moved` flag to true to indicate that movement should occur in `update` and `render`
    switch (event->key.key)
    {
    case SDLK_UP:
    case SDLK_W:
      moved = true;
      new_y = NEW_POS(max, player_y - 1, 0);
      break;
    case SDLK_DOWN:
    case SDLK_S:
      moved = true;
      new_y = NEW_POS(min, player_y + 1, map_height - 1);
      break;
    case SDLK_LEFT:
    case SDLK_A:
      moved = true;
      new_x = NEW_POS(max, player_x - 1, 0);
      break;
    case SDLK_RIGHT:
    case SDLK_D:
      moved = true;
      new_x = NEW_POS(min, player_x + 1, map_width - 1);
      break;
    default:
      break; // ignore other keys
    }

#undef NEW_POS

    if (moved)
    {
      Log::info("Player moved to ({}, {})", new_x, new_y);
      player_x_old = player_x;
      player_y_old = player_y;
      player_x = new_x;
      player_y = new_y;
    }
  }

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
  if (moved)
  {
    //TODO this assumes that the player character always exists and is unique,
    // change this to verify first before moving! but this will all get replaced at some point anyway
    // first, replace map location from '@' to '.':
    map.replace(map.find(player_char), 1, ".");
    // then, replace new map location with '@':
    // calculate string index from coordinates
    int str_i = (player_y * (map_width + 1)) + player_x; // +1 for newline character at end of each row
    map.replace(str_i, 1, std::string(1, player_char));

    // update the map texture
    if (SDL_AppResult result = generate_map_texture())
    {
      return result;
    }

    // reset flag
    moved = false;
  }

  // ensure bounds isn't null
  if (bounds == nullptr)
    return Log::error("Bounds cannot be null in GameWorld::render");

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
      return Log::error("Failed to render map texture: {}", SDL_GetError());
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult GameWorld::generate_map_texture()
{
  // Generate surface from text
  SDL_Surface *map_surface = TTF_RenderText_Blended_Wrapped(m_game->font(), map.c_str(), 0, {255, 255, 255, 255}, 0);
  if(map_surface == nullptr)
  {
    return Log::error_init("map_surface");
  }
  // destroy map_texture if it exists already
  if (map_texture != nullptr)
  {
    SDL_DestroyTexture(map_texture);
    map_texture = nullptr;
  }
  // allocate the texture
  map_texture = SDL_CreateTextureFromSurface(m_game->renderer(), map_surface);
  SDL_DestroySurface(map_surface);
  if(map_texture == nullptr)
  {
    return Log::error_init("map_texture");
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult GameWorld::generate_world_map()
{
  Log::info("generate_world_map() called, running test_world_map_operations()...");
  //TODO replace with actual WorldMap generation
  return SDL_APP_CONTINUE; // skip for now
}

SDL_AppResult GameWorld::test_world_map_operations()
{
  auto chunk = world_map.create_chunk(0, 0, 0);
  if (chunk == entt::null)
  {
    return Log::error("Failed to create initial chunk at {{0, 0, 0}}");
  }
  Log::info("Created initial chunk at {{0, 0, 0}}");
  Log::info(1, "Chunk entity ID: {}", static_cast<entt::id_type>(chunk));
  auto& chunk_pos = world_map.get_component<ChunkPos>(chunk);
  Log::info(2, "Chunk position component: {{{}, {}, {}}}",
    chunk_pos.x, chunk_pos.y, chunk_pos.z);

  auto chunk_negative = world_map.create_chunk(-4, -4, -4);
  if (chunk_negative == entt::null)
  {
    return Log::error("Failed to create chunk at {{-4, -4, -4}}");
  }
  Log::info("Created chunk at {{-4, -4, -4}}");
  Log::info(1, "Chunk entity ID: {}", static_cast<entt::id_type>(chunk_negative));
  auto& chunk_negative_pos = world_map.get_component<ChunkPos>(chunk_negative);
  Log::info(2, "Chunk position component: {{{}, {}, {}}}",
    chunk_negative_pos.x, chunk_negative_pos.y, chunk_negative_pos.z);

  // global coords, so should be in chunk (-4, -4, -4)
  // and at local coords (11, 6, 1)
  Sint64 x = (ChunkData::SIZE * -3) - 5;
  Sint64 y = (ChunkData::SIZE * -3) - 10;
  Sint64 z = (ChunkData::SIZE * -3) - 15;
  BlockID block = world_map.get_block(x, y, z);
  Log::info(1, "get_block({}, {}, {}) = {}", x, y, z, block);

  // ---------------------- let's test all operations!!! -----------------------

  // ------ create_chunk ------
  auto chunk_test1 = world_map.create_chunk(1, 2, 3);
  Log::info("Attempted to create chunk at {{1, 2, 3}}: {}",
    chunk_test1 == entt::null ? "Failed!" : "Succeeded");
  // Check failure case: creating a chunk that already exists
  auto chunk_test2 = world_map.create_chunk(1, 2, 3);
  // should return a null entity here, let's confirm:
  Log::info(1, "Attempted to create duplicate chunk at {{1, 2, 3}}: {}",
    chunk_test2 == entt::null ? "got null entity as expected"
                              : "UNEXPECTED: got a valid entity!");
  // Let's make one more:
  auto chunk_test3 = world_map.create_chunk(-1, -2, -3);
  Log::info(1, "Attempted to create chunk at {{-1, -2, -3}}: {}",
    (chunk_test3 == entt::null ? "Failed!" : "Succeeded"));
  // We should have a chunk at {1, 2, 3}, origin at (16, 32, 48) in global coords
  // (origin is bottom-left-near corner)
  // this is currently default initialized to a 16x16x16 cube of air (block ID 0)
  // let's confirm this:

  // ------ get_chunk ------
  auto chunk_entity = world_map.get_chunk(1, 2, 3);
  Log::info(1, "Retrieved chunk: {}",
    chunk_entity == entt::null ? "Failed!" : "Succeeded");

  // ------ delete_chunk ------
  bool delete_result = world_map.delete_chunk(-1, -2, -3);
  Log::info(1, "Deleted chunk at {{-1, -2, -3}}: {}", delete_result ? "success" : "failure");
  // confirm deletion by trying to get the chunk again
  auto deleted_chunk_entity = world_map.get_chunk(-1, -2, -3);
  Log::info(1, "Attempted to get deleted chunk at {{-1, -2, -3}}, {}: {}",
    deleted_chunk_entity == entt::null ? "got null entity as expected"
                                       : "UNEXPECTED: got a valid entity!");

  // ------ get_block ------
  // Chunk-local coordinates
  auto block1 = world_map.get_block(ChunkPos{1, 2, 3}, LocalPos{0, 0, 0});
  Log::info(1, "Got block1: {}", block1);
  auto block2 = world_map.get_block(ChunkPos{1, 2, 3}, 15, 15, 15);
  Log::info(1, "Got block2: {}", block2);
  // Global coordinates
  auto block3 = world_map.get_block(GlobalPos{16, 32, 48}); // should be same as block1
  Log::info(1, "Got block3: {}", block3);
  auto block4 = world_map.get_block(31, 47, 63); // should be same as block2
  Log::info(1, "Got block4: {}", block4);


  // ------ set_block ------
  // Chunk-local coordinates
  auto block5 = world_map.set_block(ChunkPos{1, 2, 3}, LocalPos{0, 0, 0}, GROUND);
  Log::info(1, "Set block5 to GROUND: {}", block5);
  auto block6 = world_map.set_block(ChunkPos{1, 2, 3}, 15, 15, 15, BlockID{2});
  Log::info(1, "Set block6 to 2: {}", block6);
  // Global coordinates
  auto block7 = world_map.set_block(GlobalPos{16, 32, 48}, BlockID{3});
  Log::info(1, "Set block7 to 3: {}", block7);
  auto block8 = world_map.set_block(31, 47, 63, BlockID{4});
  Log::info(1, "Set block8 to 4: {}", block8);
  // after all this, GlobalPos(16, 32, 48) should be block ID 3
  // and GlobalPos(31, 47, 63) should be block ID 4
  // let's confirm:
  auto block9 = world_map.get_block(16, 32, 48);
  Log::info(1, "Got block9 (should be 3): {}", block9);
  auto block10 = world_map.get_block(31, 47, 63);
  Log::info(1, "Got block10 (should be 4): {}", block10);

  return SDL_APP_CONTINUE;
}


} // namespace bipsy::gaiasim
