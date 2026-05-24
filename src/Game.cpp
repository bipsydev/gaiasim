#include "Game.hpp"

#include "screens/ScreenTest.hpp"

#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <cassert>
#include <string>
#include <format>


namespace bipsy::gaiasim
{


Game::Game(InitRequest initializations)
: m_screens{},
  m_active_screen_index{0},
  m_clear_color{GAME_CLEAR_COLOR_DEFAULT},
  m_window{nullptr},
  m_renderer{nullptr},
  m_font{nullptr},
  m_frame{0},
  m_time_ns{0},
  m_delta_time_ns{0},
  m_inits_complete{NONE}
{
  init(initializations);  // throws away return value `SDL_AppResult`
}


SDL_AppResult Game::init(InitRequest initializations)
{

  assert(initializations <= Game::InitRequest::ALL);

  // Perform requested initializations in order
  while (m_inits_complete < initializations)
  {
    log_info("Initialization step: " + std::to_string(m_inits_complete + 1), 1);
    // the `init_` functions will update `m_inits_complete` if successful,
    // or return an error result if failed, which will break the loop.
    switch(m_inits_complete)
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
  if (m_inits_complete >= LIBRARIES)
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
  m_inits_complete = LIBRARIES;
  return SDL_APP_CONTINUE;  // indicate that we want to continue app execution
}


SDL_AppResult Game::init_system_objects()
{

  // check initialization stage to avoid re-initialization
  if (m_inits_complete >= SYSTEM_OBJECTS)
  {
    log_error("System objects already initialized!");
    return SDL_APP_FAILURE;
  }

  // Create a window
  if (not (m_window = SDL_CreateWindow(
    "gaiasim engine",
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
  SDL_SetWindowPosition(m_window,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  // Create a rendering context from a window
  if (not (m_renderer = SDL_CreateRenderer(m_window, NULL)))
  {
    return log_error_init("SDL_Renderer *renderer");
  }
  else
  {
    log_info("SDL_Renderer created successfully", 2);
  }
  // Enable adaptive vsync for the renderer
  SDL_SetRenderVSync(m_renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
  
  // Create a TTF font from PixelCode.ttf in our ./assets/ directory
  std::string font_name = "PixelCode.ttf";
  if (not (m_font = TTF_OpenFont((asset_dir(font_name) + font_name).c_str(), 24)))
  {
    return log_error_init("TTF_Font *font");
  }
  else
  {
    log_info("TTF_Font \"" + std::string(font_name) + "\" loaded successfully", 2);
  }

  // Initialization complete, update stage and return
  m_inits_complete = SYSTEM_OBJECTS;
  return SDL_APP_CONTINUE;
}


SDL_AppResult Game::init_game_state()
{
  // check initialization stage to avoid re-initialization
  if (m_inits_complete >= GAME_STATE)
  {
    log_error("Game state already initialized!");
    return SDL_APP_FAILURE;
  }

  // Create a `ScreenTest` and add it to the list of screens
  Screen *screen_test = new ScreenTest(this);
  if (SDL_AppResult result = screen_test->init())
  {
    log_error("Error occured while initializing `ScreenTest`, terminating...");
    return result;
  }
  m_screens.push_back(screen_test);
  SDL_SetWindowTitle(m_window, ("gaiasim - " + screen_test->name()).c_str());
  // `active_screen_index` is already 0, so `screen_test` is the active screen

  // Initialization complete, update stage and return
  m_inits_complete = GAME_STATE;
  return SDL_APP_CONTINUE;
}



Game::~Game()
{
  log_info("Deinitializing `Game` object...");

  // Deinitialization Stage 3: Game state
  if (m_inits_complete >= GAME_STATE)
  {
    log_info("Deinitializing game state...", 1);
    // ScreenTest needs to be deallocated
    // Deallocate all screens we have active
    for (Screen *screen : m_screens)
    {
      log_info("Deallocating screen \"" + screen->name() + "\"...", 2);
      delete screen;
    }
    // clear out the pointer vector
    m_screens.clear();
  }

  // Deinitialization Stage 2: System objects
  if (m_inits_complete >= SYSTEM_OBJECTS)
  {
    log_info("Deinitializing system objects...", 1);
    if (m_renderer)
    {
      SDL_DestroyRenderer(m_renderer);
      m_renderer = nullptr;
    }
    if (m_window)
    {
      SDL_DestroyWindow(m_window);
      m_window = nullptr;
    }
  }

  // Deinitialization Stage 1: Libraries
  if (m_inits_complete >= LIBRARIES)
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
  // update delta time
  // uses previous time_ns before we update in next line
  m_delta_time_ns = SDL_GetTicksNS() - m_time_ns;

  // update frame time
  m_time_ns = SDL_GetTicksNS();

  // log frame count (with padding)
  auto frame_count = std::to_string(m_frame);
  frame_count.resize(5, ' '); // Pad frame count to 5 characters for better readability

  // For Android, just log every 60th frame
  // to avoid spamming the logcat with too many messages
  #if __ANDROID__
  if (m_frame % 60 == 0)
  {
    log_info_enable(true);
  }
  else if (m_frame % 60 == 1)
  {
    log_info_enable(false);
  }
#endif // __ANDROID__

  log_info("--- frame: " + frame_count + " ---");
  log_info(std::format("Frame time (s): {:.2f} s", m_time_ns / 1000000000.0));
  log_info(std::format("Delta time (ms): {:.4f} ms", m_delta_time_ns / 1000000.0));
  log_info(std::format("FPS: {:.2f}", fps()));

  // Make window visible on first frame (after initialization)
  if (m_frame == 0)
  {
    SDL_ShowWindow(m_window);
    log_info("Window shown");
  }

  // forward to active screen
  return active_screen()->update();
}

SDL_AppResult Game::render()
{
  // Clear the screen with a solid color
  SDL_SetRenderDrawColor(m_renderer,
      m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
      
  SDL_RenderClear(m_renderer);
  // --- Drawing block ---
  {
    active_screen()->render(m_renderer);
  }
  // Present the rendered frame buffer to the screen
  SDL_RenderPresent(m_renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult Game::post_render_update()
{

  auto result = active_screen()->post_render_update();

  // increment frame count and return (continue running to next callback)
  m_frame++;
  
  return result;
}

} // namespace bipsy::gaiasim