#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_TEST_HPP
#define BIPSY_GAIASIM_SCREEN_TEST_HPP


#include "Screen.hpp"

#include "Game.hpp"

#include "SDL3/SDL.h"


namespace bipsy::gaiasim
{

/**
 * @brief Test Screen, simply draws some gradient rectangles and test data.
 * 
 * Used just to test rendering & the `Screen` switching system.
 */
class ScreenTest : public Screen
{
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
  SDL_Vertex polygon2[VERTEX_COUNT]; // Vertices for gradient rectangle 2

  // Here we use 4 triangles (12 vertices) to render a rectangle
  // with a better looking gradient effect
  static const int GRADIENT_RECT_TRIANGLE_COUNT = 4;
  static const int GRADIENT_RECT_VERTEX_COUNT = GRADIENT_RECT_TRIANGLE_COUNT * 3;
  SDL_Vertex gradient_rect[GRADIENT_RECT_VERTEX_COUNT];

  // texture for holding rendered text
  SDL_Texture *text_texture = nullptr;

  /**
   * @brief C String that holds the system name.
   */
  #define SYSTEM_STR inline static const char * system_str =
  #if __ANDROID__
  SYSTEM_STR "Android";
  #elif __EMSCRIPTEN__
  SYSTEM_STR "Emscripten";
  #elif __linux__
  SYSTEM_STR "Linux";
  #elif _WIN32
  SYSTEM_STR "Windows";
  #elif __APPLE__
  SYSTEM_STR "Apple";
  #else
  SYSTEM_STR "Unknown";
  #endif
  #undef SYSTEM_STR

public:
  ScreenTest(Game * game)
  : Screen(game, "ScreenTest"),
    polygon1{},
    polygon2{},
    gradient_rect{},
    text_texture{nullptr}
  { }

  // Delete copy constructor and copy assignment operator to prevent copying
  // May implement later if needed
  ScreenTest(const ScreenTest &) = delete;
  ScreenTest &operator=(const ScreenTest &) = delete;

  ~ScreenTest()
  { };

  SDL_AppResult init() override;
  void deinit() override;

  SDL_AppResult event(SDL_Event *event) override;
  SDL_AppResult update() override;
  SDL_AppResult render(SDL_Renderer *renderer) override;
  SDL_AppResult post_render_update() override;

  SDL_AppResult show() override;
  SDL_AppResult hide() override;

private:
  SDL_AppResult init_text_texture();
};

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_SCREEN_TEST_HPP