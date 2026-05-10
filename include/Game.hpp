#pragma once
#ifndef BIPSY_GAIASIM_GAME_HPP
#define BIPSY_GAIASIM_GAME_HPP


#include "SDL3/SDL.h"


namespace bipsy
{

namespace gaiasim
{

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

  
  Game();

  SDL_AppResult event(SDL_Event *event);
  SDL_AppResult update();
  SDL_AppResult render();


  ~Game();

}; // class Game

} // namespace bipsy::gaisim


namespace sdlutils
{

static inline SDL_AppResult log_error_init(const char *subsystem)
{
  SDL_LogError(SDL_LOG_CATEGORY_ERROR,
    "Failed to initialize %s: %s", subsystem, SDL_GetError());
  return SDL_APP_FAILURE;
}

static inline void log_info(const char *message)
{
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message);
}

} // namespace bipsy::sdlutils

} // namespace bipsy

#endif // BIPSY_GAIASIM_GAME_HPP