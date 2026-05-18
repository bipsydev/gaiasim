#include "Game.hpp"

#include "screens/ScreenTest.hpp"

#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <cassert>
#include <string>


namespace bipsy::gaiasim
{


Game::Game(InitRequest initializations)
: screens{},
  active_screen_index{0},
  clear_color{0, 128, 255, 255},
  window{nullptr},
  renderer{nullptr},
  frame{0},
  time_ns{0},
  delta_time_ns{0},
  inits_complete{NONE}
{
  init(initializations);  // throws away return value `SDL_AppResult`
}


SDL_AppResult Game::init(InitRequest initializations)
{

  assert(initializations <= Game::InitRequest::ALL);

  // Perform requested initializations in order
  while (inits_complete < initializations)
  {
    log_info("Initialization step: " + std::to_string(inits_complete + 1), 1);
    // the `init_` functions will update `inits_complete` if successful,
    // or return an error result if failed, which will break the loop.
    switch(inits_complete)
    {
    case NONE:
      // Initialization Stage 1: Libraries (SDL_Init)
      if (SDL_AppResult result = init_libraries())
      {
        log_error("Error occured while initializing libraries, terminating...");
        return result;
      }
      log_info("Libraries initialized successfully", 1);
      break;
    case LIBRARIES:
      // Initialization Stage 2: System objects (window, renderer, etc)
      if (SDL_AppResult result = init_system_objects())
      {
        log_error("Error occured while initializing system objects, terminating...");
        return result;
      }
      log_info("System objects initialized successfully", 1);
      break;
    case SYSTEM_OBJECTS:
      // Initialization Stage 3: Game state (initial screen, asset allocation)
      if (SDL_AppResult result = init_game_state())
      {
        log_error("Error occured while initializing game state, terminating...");
        return result;
      }
      log_info("Game state initialized successfully", 1);
      break;
    case GAME_STATE:
      // error state, code should never reach here
      assert(false && "Invalid initialization state reached in Game constructor!");
      // GAME_STATE should be the last initialization step, unless we modified it
      static_assert(GAME_STATE == ALL, "Did we add a new initialization step without updating the constructor?");
    }
  }

  log_info("`Game` object constructed successfully!");
  return SDL_APP_CONTINUE;
}


SDL_AppResult Game::init_libraries()
{
  // check initialization stage to avoid re-initialization
  if (inits_complete >= LIBRARIES)
  {
    log_error("Libraries already initialized!");
    return SDL_APP_FAILURE;
  }

  // Initialize SDL and its subsystems
  if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
  {
    return log_error_init("SDL");
  }
  else
  {
    log_info("SDL3 initialized successfully", 2);
  }

  if (not TTF_Init())
  {
    return log_error_init("SDL_ttf");
  }
  else
  {
    log_info("SDL_ttf initialized successfully", 3);
  }

  if (not MIX_Init())
  {
    return log_error_init("SDL_mixer");
  }
  else
  {
    log_info("SDL_mixer initialized successfully", 3);
  }

  log_info("All subsystems initialized successfully", 2);

  // Initialization complete, update stage and return
  inits_complete = LIBRARIES;
  return SDL_APP_CONTINUE;  // indicate that we want to continue app execution
}


SDL_AppResult Game::init_system_objects()
{

  // check initialization stage to avoid re-initialization
  if (inits_complete >= SYSTEM_OBJECTS)
  {
    log_error("System objects already initialized!");
    return SDL_APP_FAILURE;
  }

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
    log_info("SDL_Window created successfully", 2);
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
    log_info("SDL_Renderer created successfully", 2);
  }
  // Enable adaptive vsync for the renderer
  SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
  
  // Initialization complete, update stage and return
  inits_complete = SYSTEM_OBJECTS;
  return SDL_APP_CONTINUE;
}


SDL_AppResult Game::init_game_state()
{
  // check initialization stage to avoid re-initialization
  if (inits_complete >= GAME_STATE)
  {
    log_error("Game state already initialized!");
    return SDL_APP_FAILURE;
  }

  // Create a `ScreenTest` and add it to the list of screens
  Screen *screen_test = new ScreenTest();
  if (SDL_AppResult result = screen_test->init())
  {
    log_error("Error occured while initializing `ScreenTest`, terminating...");
    return result;
  }
  screens.push_back(screen_test);
  // `active_screen_index` is already 0, so `screen_test` is the active screen

  // Initialization complete, update stage and return
  inits_complete = GAME_STATE;
  return SDL_APP_CONTINUE;
}



Game::~Game()
{
  log_info("Deinitializing `Game` object...");

  // Deinitialization Stage 3: Game state
  if (inits_complete >= GAME_STATE)
  {
    log_info("Deinitializing game state...", 1);
    // ScreenTest needs to be deallocated
    // Deallocate all screens we have active
    for (Screen *screen : screens)
    {
      log_info("Deallocating screen \"" + screen->name() + "\"...", 2);
      delete screen;
    }
    // clear out the pointer vector
    screens.clear();
  }

  // Deinitialization Stage 2: System objects
  if (inits_complete >= SYSTEM_OBJECTS)
  {
    log_info("Deinitializing system objects...", 1);
    if (renderer)
    {
      SDL_DestroyRenderer(renderer);
      renderer = nullptr;
    }
    if (window)
    {
      SDL_DestroyWindow(window);
      window = nullptr;
    }
  }

  // Deinitialization Stage 1: Libraries
  if (inits_complete >= LIBRARIES)
  {
    log_info("Deinitializing libraries...", 1);
    MIX_Quit();
    TTF_Quit();
    SDL_Quit();
  }

  log_info("`Game` object deinitialized successfully!");
}



SDL_AppResult Game::event(SDL_Event *event)
{
  // catch the QUIT event (game requesting successful termination)
  if (event->type == SDL_EVENT_QUIT)
  {
    log_info("Quit event received, terminating...");
    return SDL_APP_SUCCESS;
  }
  else
  {
    // forward to active screen
    active_screen()->event(event);
  }

  return SDL_APP_CONTINUE;
}


SDL_AppResult Game::iterate()
{
  // Split `iterate` into update/render functions for organization
  if (SDL_AppResult result = update())
  {
    return result; // Return if update requested termination
  }
  if (SDL_AppResult result = render())
  {
    return result;
  }
  if (SDL_AppResult result = post_render_update())
  {
    return result;
  }

  return SDL_APP_CONTINUE;
}


SDL_AppResult Game::update()
{
  // update frame time
  time_ns = SDL_GetTicksNS();

  // log frame count (with padding)
  auto frame_count = std::to_string(frame);
  frame_count.resize(5, ' '); // Pad frame count to 5 characters for better readability
  log_info(("--- frame: " + frame_count + " ---").c_str());
  log_info(("Frame time (s): " + std::to_string(time_ns / 1000000000.0)).c_str());
  
  // Make window visible on first frame (after initialization)
  if (frame == 0)
  {
    SDL_ShowWindow(window);
    log_info("Window shown");
  }

  // forward to active screen
  return active_screen()->update();
}

SDL_AppResult Game::render()
{
  // Clear the screen with a solid color
  SDL_SetRenderDrawColor(renderer,
      clear_color.r, clear_color.g, clear_color.b, clear_color.a);
      
  SDL_RenderClear(renderer);
  // --- Drawing block ---
  {
    active_screen()->render(renderer);
  }
  // Present the rendered frame buffer to the screen
  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult Game::post_render_update()
{

  auto result = active_screen()->post_render_update();

  // increment frame count and return (continue running to next callback)
  frame++;
  
  return result;
}

} // namespace bipsy::gaiasim