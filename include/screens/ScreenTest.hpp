#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_TEST_HPP
#define BIPSY_GAIASIM_SCREEN_TEST_HPP


#include "Screen.hpp"

#include "Game.hpp"

#include "SDL3/SDL.h"

#include <string>


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
  SDL_Vertex m_polygon1[VERTEX_COUNT]; // Vertices for gradient rectangle 1
  SDL_Vertex m_polygon2[VERTEX_COUNT]; // Vertices for gradient rectangle 2

  // Here we use 4 triangles (12 vertices) to render a rectangle
  // with a better looking gradient effect
  static const int GRADIENT_RECT_TRIANGLE_COUNT = 4;
  static const int GRADIENT_RECT_VERTEX_COUNT = GRADIENT_RECT_TRIANGLE_COUNT * 3;
  SDL_Vertex m_gradient_rect[GRADIENT_RECT_VERTEX_COUNT];

  // Verticies for a rainbow, color-changing triangle
  SDL_Vertex m_rainbow_triangle[3];

  // texture for holding rendered text
  SDL_Texture *m_text_texture = nullptr;

  std::string m_text_str; // This holds the current text string that is rendered in `m_text_texture`. We keep track of this so we can avoid regenerating the texture if the text hasn't changed.

  /**
   * @brief C String that holds the system name.
   */
  #define SYSTEM_STR inline static const char * m_system_str =
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
    m_polygon1{},
    m_polygon2{},
    m_gradient_rect{},
    m_rainbow_triangle{},
    m_text_texture{nullptr},
    m_text_str{""}
  { }

  // Delete copy constructor and copy assignment operator to prevent copying
  // May implement later if needed
  ScreenTest(const ScreenTest &) = delete;
  ScreenTest &operator=(const ScreenTest &) = delete;

  virtual ~ScreenTest();

  SDL_AppResult init() override;

  SDL_AppResult event(SDL_Event *event) override;
  SDL_AppResult update() override;
  SDL_AppResult render(SDL_Renderer *renderer) override;
  SDL_AppResult post_render_update() override;

  SDL_AppResult show() override;
  SDL_AppResult hide() override;

private:
  SDL_AppResult init_text_texture();
  SDL_AppResult switch_to_main_screen();
};

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_SCREEN_TEST_HPP