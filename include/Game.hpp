#pragma once
#ifndef BIPSY_GAIASIM_GAME_HPP
#define BIPSY_GAIASIM_GAME_HPP


#include "SDL_indent.hpp"
#include "Screen.hpp"

#include "SDL3/SDL.h"

#include <string>
#include <vector>


namespace bipsy
{


namespace sdlutils
{

static inline SDL_AppResult log_error_init(std::string subsystem)
{
  SDL_LogError(SDL_LOG_CATEGORY_ERROR,
    "Failed to initialize %s: %s", subsystem.c_str(), SDL_GetError());
  return SDL_APP_FAILURE;
}

static inline void log_info(std::string message, int indent = 0)
{
  SDL_LogIndent(indent, message.c_str());
}

static inline void log_error(std::string message)
{
  SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", message.c_str());
}

} // namespace bipsy::sdlutils


namespace gaiasim
{
  using namespace bipsy::sdlutils;  // log_info, log_error_init

/**
 * @brief Local application running state structure.
 * 
 * Contains pointers to SDL objects for window, renderer, etc along with
 * other local application running state information. This object is
 * initialized once in SDL_AppInit and then passed around in the rest of the
 * SDL callback entrypoints as a `void*` (must be cast into a `Appstate*`).
 */
class Game
{
  // List of all allocated screens (may or may not be active)
  std::vector<Screen *> screens;
  // Index of the currently active screen in `screens`
  Uint8 active_screen_index = 0;

public:

  SDL_Window   *window   = nullptr; // Window object
  SDL_Renderer *renderer = nullptr; // Rendering context to window

  /*
   * -- Vertex data for 2D shape rendering --
   * `SDL_RenderGeometry` uses an array of `SDL_Vertex` objects to specify
   * the vertices of the shape to be rendered.
   * It requires the vertex count to be in multiples of 3 as it renders
   * each group of 3 vertices as a triangle. So for example, to render a rectangle
   * using 2 triangles, we need 6 vertices (3 for each triangle).
  */
  static const int VERTEX_COUNT = 6;
  SDL_Vertex polygon1[VERTEX_COUNT]; // Vertices for gradient rectangle 1
#if not __ANDROID__
  SDL_Vertex polygon2[VERTEX_COUNT]; // Vertices for gradient rectangle 2
#endif // __ANDROID__

  // Here we use 4 triangles (12 vertices) to render a rectangle
  // with a better looking gradient effect
  static const int GRADIENT_RECT_TRIANGLE_COUNT = 4;
  static const int GRADIENT_RECT_VERTEX_COUNT = GRADIENT_RECT_TRIANGLE_COUNT * 3;
  SDL_Vertex gradient_rect[GRADIENT_RECT_VERTEX_COUNT];

  // -- Time-related variables --
  int frame = 0;      // Frame count
  Uint64 time_ns = 0; // Time (nanoseconds) since SDL initialization
                      // (updated at the beginning of each frame)
  Uint64 delta_time_ns = 0; // Time (nanoseconds) taken to render the previous frame

  
  /**
   * @brief Flags for requesting certain initialization steps in the constructor.
   * 
   * Each consecutive flag requires previous steps to also be initialized.
   * The order is as follows:
   * 
   *        LIBRARIES -> SYSTEM_OBJECTS -> GAME_STATE
   * 
   * So for example, if you request GAME_STATE initialization, it will
   * also initialize SYSTEM_OBJECTS and LIBRARIES in order before
   * initializing GAME_STATE.
   * 
   * LIBRARIES:      SDL, it's subsystems, and any other external libraries.
   * 
   * SYSTEM_OBJECTS: Global SDL/system objects like window, renderer, etc that are
   *                 allocated for the entire duration of the `Game`'s lifetime.
   * 
   * GAME_STATE:     The initial game state, which includes loading assets and
   *                 starting an initial beginning scene.
   */
  enum InitRequest : Uint8
  {
    NONE            = 0U,
    LIBRARIES       = 1U,
    SYSTEM_OBJECTS  = 2U,
    GAME_STATE      = 3U,
    ALL             = 3U, // Same as GAME_STATE
  };

  /**
   * @brief Construct a new Game object.
   * 
   * This also calls every `init_*` function in order by default.
   * 
   * @param initializations Flags for requesting certain initialization steps in the constructor.
   * 
   */
  Game(InitRequest initializations = ALL);

  SDL_AppResult init_libraries();
  SDL_AppResult init_system_objects();
  SDL_AppResult init_game_state();

  SDL_AppResult event(SDL_Event *event);
  SDL_AppResult iterate();
  SDL_AppResult update();
  SDL_AppResult render();
  SDL_AppResult post_render_update();


  ~Game();

}; // class Game

} // namespace bipsy::gaisim

} // namespace bipsy

#endif // BIPSY_GAIASIM_GAME_HPP