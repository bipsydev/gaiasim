#include "Game.hpp"

#include "screens/ScreenTest.hpp"
#include "SDL3_utils.hpp"

#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"

#include <cassert>
#include <string>
#include <format>


namespace bipsy::gaiasim
{

using namespace bipsy::sdl3_utils; // Log, GAME_CLEAR_COLOR_DEFAULT


// Factory Method
SDL_AppResult Game::new_game(void *&appstate, InitRequest initializations)
{
  LOG_FRAME_CLASS(Game);
  Log::verbose("Requested initializations = {}", initializations);

  Log::verbose("Constructing a Game instance without initializations first");
  Game *game = new Game(Game::InitRequest::NONE);
  // Assign the address to the appstate in-param
  appstate = game;
  Log::verbose("New Game instance bound to appstate pointer, now initializing Game with requested initializations...");
  // initialize game state up to the requested init phase, and return result
  SDL_AppResult result = game->init(initializations);

  // log and return
  Log::verbose("Game::new_game factory method completed initializing Game {} returning {}",
               (result == SDL_APP_CONTINUE) ? "successfully," :
               (result == SDL_APP_SUCCESS) ? "with a \"successful\" termination request? weird..." :
               (result == SDL_APP_FAILURE) ? "with failure," : "with unknown result!",
               result);
  return result; // return result from initialization request
}


// Constructor
Game::Game(InitRequest initializations)
: m_screens{},
  m_active_screen_index{0},
  m_clear_color{GAME_CLEAR_COLOR_DEFAULT},
  m_window{nullptr},
  m_renderer{nullptr},
  m_font{nullptr},
  m_font_small{nullptr},
  m_font_large{nullptr},
  m_frame{0},
  m_time_ns{0},
  m_delta_time_ns{0},
  m_inits_complete{NONE}
{
  LOG_FRAME_CLASS(Game);
  // only initialize if we requested something to initialize
  if (initializations > InitRequest::NONE)
  {
    // throws away return value `SDL_AppResult`
    init(initializations);
  }
  else
  {
    Log::verbose("No initializations requested, skipping initialization");
  }
}


SDL_AppResult Game::init(InitRequest initializations)
{
  LOG_FRAME_CLASS(Game);
  Log::verbose("Requested initializations = {}", initializations);

  // ensure we have initializations to actually do
  assert(initializations <= Game::InitRequest::ALL);

  // Perform requested initializations in order
  while (m_inits_complete < initializations)
  {
    Log::verbose("Performing initialization step: {}", m_inits_complete + 1);
    // the `init_` functions will update `m_inits_complete` if successful,
    // or return an error result if failed, which will break the loop.
    switch(m_inits_complete)
    {
    case NONE:
      // Initialization Stage 1: Libraries (SDL_Init)
      if (SDL_AppResult result = init_libraries())
      {
        Log::error("Error occured while initializing libraries, terminating...");
        return result;
      }
      Log::verbose("Libraries initialized successfully");
      break;
    case LIBRARIES:
      // Initialization Stage 2: System objects (window, renderer, etc)
      if (SDL_AppResult result = init_system_objects())
      {
        Log::error("Error occured while initializing system objects, terminating...");
        return result;
      }
      Log::verbose("System objects and global assets initialized successfully");
      break;
    case SYSTEM_OBJECTS:
      // Initialization Stage 3: Game state (initial screen, asset allocation)
      if (SDL_AppResult result = init_game_state())
      {
        Log::error("Error occured while initializing game state, terminating...");
        return result;
      }
      Log::verbose("Game state initialized successfully");
      break;
    case GAME_STATE:
      // error state, code should never reach here
      assert(false && "Invalid initialization state reached in Game constructor!");
      // GAME_STATE should be the last initialization step, unless we modified it
      static_assert(GAME_STATE == ALL, "Did we add a new initialization step without updating the constructor?");
    }
  }

  return SDL_APP_CONTINUE;
}


SDL_AppResult Game::init_libraries()
{
  LOG_FRAME_CLASS(Game);

  // check initialization stage to avoid re-initialization
  if (m_inits_complete >= LIBRARIES)
  {
    Log::error("Libraries already initialized!");
    return SDL_APP_FAILURE;
  }
  Log::verbose("Initializing system libraries...");

  // Initialize SDL and its subsystems
  if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
  {
    return Log::error_init("SDL");
  }
  Log::verbose("SDL3 initialized successfully, initializing subsystems...");

  if (not TTF_Init())
  {
    return Log::error_init("SDL_ttf");
  }
  Log::verbose(Log::indent() + 1, "SDL_ttf initialized successfully");

  if (not MIX_Init())
  {
    return Log::error_init("SDL_mixer");
  }
  Log::verbose(Log::indent() + 1, "SDL_mixer initialized successfully");

  Log::verbose("All subsystems initialized successfully");

  // Initialization complete, update stage and return
  m_inits_complete = LIBRARIES;
  return SDL_APP_CONTINUE;  // indicate that we want to continue app execution
}


SDL_AppResult Game::init_system_objects()
{
  LOG_FRAME_CLASS(Game);

  // check initialization stage to avoid re-initialization
  if (m_inits_complete >= SYSTEM_OBJECTS)
  {
    Log::error("System objects already initialized!");
    return SDL_APP_FAILURE;
  }
  Log::verbose("Initializing system objects and global assets...");

  // Create a window
  if (not (m_window = SDL_CreateWindow(
    "gaiasim engine",
    1280, 720,
    // SDL_WINDOW_VULKAN | 
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN)))
  {
    return Log::error_init("SDL_Window *window");
  }
  else
  {
    Log::verbose("SDL_Window created successfully");
  }
  // Center the window on the screen
  // This takes a second, so we wait to show the window
  // until the first frame in SDL_AppIterate
  SDL_SetWindowPosition(m_window,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  // Create a rendering context from a window
  if (not (m_renderer = SDL_CreateRenderer(m_window, NULL)))
  {
    return Log::error_init("SDL_Renderer *renderer");
  }
  else
  {
    Log::verbose("SDL_Renderer created successfully");
  }
  // Enable adaptive vsync for the renderer
  SDL_SetRenderVSync(m_renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
  
  // Create a TTF font from PixelCode.ttf in our ./assets/ directory
  std::string font_name = "PixelCode.ttf";
  if (not (m_font = TTF_OpenFont(asset_dir(font_name).c_str(), 24)))
  {
    return Log::error_init("TTF_Font *font");
  }
  else
  {
    Log::verbose("TTF_Font \"{}\" loaded at size {} successfully", font_name, 24);
  }

  // Create a smaller TTF font from PixelCode.ttf in our ./assets/ directory
  if (not (m_font_small = TTF_OpenFont(asset_dir(font_name).c_str(), 14)))
  {
    return Log::error_init("TTF_Font *font_small");
  }
  else
  {
    Log::verbose("TTF_Font \"{}\" loaded at size {} successfully", font_name, 14);
  }

  // Create larger TTF font
  // TODO dynamic font size based on DPI scale? right now it's just assuming 2.0 scale and doubling
  if (not (m_font_large = TTF_OpenFont(asset_dir(font_name).c_str(), 48)))
  {
    return Log::error_init("TTF_Font *font_large");
  }
  else
  {
    Log::verbose("TTF_Font \"{}\" loaded at size {} successfully", font_name, 48);
  }

  // Initialization complete, update stage and return
  m_inits_complete = SYSTEM_OBJECTS;
  return SDL_APP_CONTINUE;
}


SDL_AppResult Game::init_game_state()
{
  LOG_FRAME_CLASS(Game);

  // check initialization stage to avoid re-initialization
  if (m_inits_complete >= GAME_STATE)
  {
    Log::error("Game state already initialized!");
    return SDL_APP_FAILURE;
  }
  Log::verbose("Setting initial game state...");

  // Create a `ScreenTest` and add it to the list of screens
  Log::verbose("For now, we're going to initialize at a new ScreenTest screen instance:");
  Screen *screen_test = new ScreenTest(this);
  if (SDL_AppResult result = screen_test->init())
  {
    Log::critical("Error occured while initializing `ScreenTest`, terminating...");
    return result;
  }
  // Now we can add it to our list of screens and update the window title
  m_screens.push_back(screen_test);
  SDL_SetWindowTitle(m_window, ("gaiasim - " + screen_test->name()).c_str());
  // `active_screen_index` is already 0, so `screen_test` is the active screen
  //TODO use switch_screen() function? encapsulate the window title setting logic
  Log::verbose("ScreenTest set as active screen within Game");

  Log::verbose("Initial game state has been set successfully, have fun!");
  // Initialization complete, update stage and return
  m_inits_complete = GAME_STATE;
  return SDL_APP_CONTINUE;
}



Game::~Game()
{
  LOG_FRAME_CLASS(Game);

  Log::verbose("Deinitializing `Game` object...");

  // Deinitialization Stage 3: Game state
  if (m_inits_complete >= GAME_STATE)
  {
    Log::info(Log::indent() + 1, "Deinitializing game state...");
    // ScreenTest needs to be deallocated
    // Deallocate all screens we have active
    for (Screen *screen : m_screens)
    {
      Log::info(Log::indent() + 2, "Deallocating screen \"{}\"...", screen->name());
      delete screen;
    }
    // clear out the pointer vector
    m_screens.clear();
  }

  // Deinitialization Stage 2: System objects
  if (m_inits_complete >= SYSTEM_OBJECTS)
  {
    Log::info(Log::indent() + 1, "Deinitializing system objects...");
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
    if (m_font)
    {
      TTF_CloseFont(m_font);
      m_font = nullptr;
    }
    if (m_font_small)
    {
      TTF_CloseFont(m_font_small);
      m_font_small = nullptr;
    }
    if (m_font_large)
    {
      TTF_CloseFont(m_font_large);
      m_font_large = nullptr;
    }
  }

  // Deinitialization Stage 1: Libraries
  if (m_inits_complete >= LIBRARIES)
  {
    Log::info(Log::indent() + 1, "Deinitializing libraries...");
    MIX_Quit();
    TTF_Quit();
    SDL_Quit();
  }

  Log::info("`Game` object deinitialized successfully!");
}



SDL_AppResult Game::event(SDL_Event *event)
{
  // catch the QUIT event (game requesting successful termination)
  if (event->type == SDL_EVENT_QUIT)
  {
    LOG_FRAME_CLASS(Game);
    Log::info("Quit event received, terminating...");
    return SDL_APP_SUCCESS;
  }
  else
  {
    // forward to active screen
    return active_screen()->event(event);
  }
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
  LOG_FRAME_CLASS(Game);
  
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

  Log::info("--- frame: {} ---", frame_count);
  Log::info("Frame time (s): {:.2f} s", m_time_ns / 1000000000.0);
  Log::info("Delta time (ms): {:.4f} ms", m_delta_time_ns / 1000000.0);
  Log::info("FPS: {:.2f}", fps());

  // Make window visible on first frame (after initialization)
  if (m_frame == 0)
  {
    SDL_ShowWindow(m_window);
    Log::info("Window shown");
  }

  // forward to active screen
  return active_screen()->update();
}

SDL_AppResult Game::render()
{
  LOG_FRAME_CLASS(Game);
  
  // Clear the screen with a solid color
  SDL_SetRenderDrawColor(m_renderer,
      m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
      
  SDL_RenderClear(m_renderer);
  // --- Drawing block ---
  {
    if (SDL_AppResult result = active_screen()->render(m_renderer))
      return result;
  }
  // Present the rendered frame buffer to the screen
  SDL_RenderPresent(m_renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult Game::post_render_update()
{
  LOG_FRAME_CLASS(Game);

  auto result = active_screen()->post_render_update();

  // increment frame count and return (continue running to next callback)
  m_frame++;
  
  return result;
}

} // namespace bipsy::gaiasim