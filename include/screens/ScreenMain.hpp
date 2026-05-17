#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_HPP


#include "Screen.hpp"

#include "SDL3/SDL.h"


namespace bipsy::gaiasim
{

class ScreenMain : public Screen
{
  
public:
  ScreenMain();
  ~ScreenMain();

  SDL_AppResult init() override;
  void deinit() override;

  SDL_AppResult event(SDL_Event *event) override;
  SDL_AppResult update() override;
  SDL_AppResult render(SDL_Renderer *renderer) override;
  SDL_AppResult post_render_update() override;

  SDL_AppResult show() override;
  SDL_AppResult hide() override;
};

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_SCREEN_MAIN_HPP