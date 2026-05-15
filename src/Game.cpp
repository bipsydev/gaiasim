#include "Game.hpp"

#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <cassert>
#include <string>


namespace bipsy::gaiasim
{


Game::Game(InitRequest initializations)
{
  assert(initializations <= Game::InitRequest::ALL);

  // Perform requested initializations in order
  InitRequest inits_complete =  NONE;
  while (inits_complete < initializations)
  {
    log_info(("  Initialization step: " + std::to_string(inits_complete + 1)).c_str());
    switch(inits_complete)
    {
    case NONE:
      if (SDL_AppResult result = init_libraries())
      {
        log_error("Error occured while initializing libraries, terminating...");
        return;
      }
      log_info("  Libraries initialized successfully");
      inits_complete = LIBRARIES;
      break;
    case LIBRARIES:
      if (SDL_AppResult result = init_system_objects())
      {
        log_error("Error occured while initializing system objects, terminating...");
        return;
      }
      log_info("  System objects initialized successfully");
      inits_complete = SYSTEM_OBJECTS;
      break;
    case SYSTEM_OBJECTS:
      if (SDL_AppResult result = init_game_state())
      {
        log_error("Error occured while initializing game state, terminating...");
        return;
      }
      log_info("  Game state initialized successfully");
      inits_complete = GAME_STATE;
      break;
    }
  }

  log_info("`Game` object constructed successfully!");
}


SDL_AppResult Game::init_libraries()
{
  // Initialize SDL and its subsystems
  if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
  {
    return log_error_init("SDL");
  }
  else
  {
    log_info("      SDL3 initialized successfully");
  }

  if (not TTF_Init())
  {
    return log_error_init("SDL_ttf");
  }
  else
  {
    log_info("      SDL_ttf initialized successfully");
  }

  if (not MIX_Init())
  {
    return log_error_init("SDL_mixer");
  }
  else
  {
    log_info("      SDL_mixer initialized successfully");
  }

  log_info("    All subsystems initialized successfully");

  return SDL_APP_CONTINUE;  // indicate that we want to continue app execution
}

SDL_AppResult Game::init_system_objects()
{

  // Create a window
  if (not (window = SDL_CreateWindow(
    "gaiasim (SDL3)",
    1280, 720,
    // SDL_WINDOW_VULKAN | 
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN)))
  {
    return log_error_init("SDL_Window *window");
  }
  else
  {
    log_info("    SDL_Window created successfully");
  }
  // Center the window on the screen
  // This takes a second, so we wait to show the window
  // until the first frame in SDL_AppIterate
  SDL_SetWindowPosition(window,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  // Create a rendering context from a window
  if (not (renderer = SDL_CreateRenderer(window, NULL)))
  {
    return log_error_init("SDL_Renderer *renderer");
  }
  else
  {
    log_info("    SDL_Renderer created successfully");
  }
  // Enable adaptive vsync for the renderer
  SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
  
  return SDL_APP_CONTINUE;
}

SDL_AppResult Game::init_game_state()
{
  // --- Initialize vertex data ---
  // Polygon 1
  log_info("    Initializing vertex data for polygon1...");
  // Triangle 1
  polygon1[0].position   = {100, 100};         // top-left vertex
  polygon1[0].color      = {1.0, 0, 0, 1.0};   // red
  polygon1[1].position   = {300, 100};         // top-right vertex
  polygon1[1].color      = {0, 1.0, 0, 1.0};   // green
  polygon1[2].position   = {300, 300};         // bottom-right vertex
  polygon1[2].color      = {0, 0, 1.0, 1.0};   // blue
  // Triangle 2 (shares top-left and bottom-right vertices with triangle 1)
  polygon1[3].position   = {100, 100};         // top-left vertex (same as vertex 0)
  polygon1[3].color      = {1.0, 0, 0, 1.0};   // red (same as vertex 0)
  polygon1[4].position   = {100, 300};         // bottom-left vertex
  polygon1[4].color      = {1.0, 1.0, 0, 1.0}; // yellow 
  polygon1[5].position   = {300, 300};         // bottom-right vertex (same as vertex 2)
  polygon1[5].color      = {0, 0, 1.0, 1.0};   // blue (same as vertex 2)


#if not __ANDROID__
  // Polygon 2
  log_info("    Initializing vertex data for polygon2...");
  // Triangle 1
  polygon2[0].position   = {400, 100};         // top-left vertex
  polygon2[0].color      = {1.0, 0, 0, 1.0};   // red
  polygon2[1].position   = {400, 300};         // bottom-left vertex
  polygon2[1].color      = {1.0, 1.0, 0, 1.0}; // yellow
  polygon2[2].position   = {600, 100};         // top-right vertex
  polygon2[2].color      = {0, 1.0, 0, 1.0};   // green

  // Triangle 2
  polygon2[3].position   = {400, 300};         // bottom-left vertex (same as vertex 1)
  polygon2[3].color      = {1.0, 1.0, 0, 1.0}; // yellow (same as vertex 1)
  polygon2[4].position   = {600, 300};         // bottom-right vertex
  polygon2[4].color      = {0, 0, 1.0, 1.0};   // blue
  polygon2[5].position   = {600, 100};         // top-right vertex (same as vertex 2)
  polygon2[5].color      = {0, 1.0, 0, 1.0};   // green (same as vertex 2
#endif // __ANDROID__



  // Gradient rectangle (4 triangles, 3 vertices each)
  log_info("    Initializing vertex data for gradient_rect...");
  // Triangle 1 (left side)
  gradient_rect[0]  = {{100, 100}, {1.0f,0.0f,1.0f,1.0f}, {0.0f,0.0f}}; // top-left vertex (magenta)
  gradient_rect[1]  = {{100, 300}, {1.0f,0.0f,0.0f,1.0f}, {0.0f,1.0f}}; // bottom-left vertex (red)
  gradient_rect[2]  = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)
  // Triangle 2 (right side)
  gradient_rect[3]  = {{300, 100}, {0.0f,1.0f,1.0f,1.0f}, {1.0f,0.0f}}; // top-right vertex (cyan)
  gradient_rect[4]  = {{300, 300}, {0.0f,1.0f,0.0f,1.0f}, {1.0f,1.0f}}; // bottom-right vertex (green)
  gradient_rect[5]  = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)
  // Triangle 3 (top side)
  gradient_rect[6]  = {{100, 100}, {1.0f,0.0f,1.0f,1.0f}, {0.0f,0.0f}}; // top-left vertex (magenta)
  gradient_rect[7]  = {{300, 100}, {0.0f,1.0f,1.0f,1.0f}, {1.0f,0.0f}}; // top-right vertex (cyan)
  gradient_rect[8]  = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)
  // Triangle 4 (bottom side)
  gradient_rect[9]  = {{100, 300}, {1.0f,0.0f,0.0f,1.0f}, {0.0f,1.0f}}; // bottom-left vertex (red)
  gradient_rect[10] = {{300, 300}, {0.0f,1.0f,0.0f,1.0f}, {1.0f,1.0f}}; // bottom-right vertex (green)
  gradient_rect[11] = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)

  log_info("    Moving gradient_rect down 300 px...");
  // Move gradient_rect down 300 px
  for (int i = 0; i < Game::GRADIENT_RECT_VERTEX_COUNT; i++)
  {
    gradient_rect[i].position.y += 300;
  }

  log_info("    Vertex data initialized successfully");

  return SDL_APP_CONTINUE;
}


SDL_AppResult Game::event(SDL_Event *event)
{
  return SDL_APP_CONTINUE;
}

SDL_AppResult Game::update()
{
  return SDL_APP_CONTINUE;
}

SDL_AppResult Game::render()
{
  return SDL_APP_CONTINUE;
}

Game::~Game()
{

}

} // namespace bipsy::gaiasim