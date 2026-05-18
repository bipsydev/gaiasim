#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_HPP


#include "Screen.hpp"

#include "Game.hpp"

#include "SDL3/SDL.h"


namespace bipsy::gaiasim
{

class ScreenMain : public Screen
{
  
public:
  ScreenMain(Game * game) : Screen(game, "ScreenMain") { };
  ~ScreenMain();

  SDL_AppResult init() override;

  SDL_AppResult event(SDL_Event *event) override;
  SDL_AppResult update() override;
  SDL_AppResult render(SDL_Renderer *renderer) override;
};

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_SCREEN_MAIN_HPP