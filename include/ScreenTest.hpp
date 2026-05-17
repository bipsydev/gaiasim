#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_TEST_HPP
#define BIPSY_GAIASIM_SCREEN_TEST_HPP

#include "Screen.hpp"

namespace bipsy::gaiasim
{

/**
 * @brief Test Screen, simply draws some gradient rectangles and test data.
 * 
 * Used just to test rendering & the `Screen` switching system.
 */
class ScreenTest : public Screen
{

public:
  ScreenTest();
  ~ScreenTest();

  SDL_AppResult init() override;
  void deinit() override;

  SDL_AppResult event(SDL_Event *event) override;
  SDL_AppResult update() override;
  SDL_AppResult render() override;
  SDL_AppResult post_render_update() override;

  SDL_AppResult show() override;
  SDL_AppResult hide() override;
};

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_SCREEN_TEST_HPP