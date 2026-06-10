#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_HPP


#include "Screen.hpp"
#include "ScreenMainGUI.hpp"
#include "Game.hpp"
#include "GameWorld.hpp"

#include "SDL3/SDL.h"

#include <string>


namespace bipsy::gaiasim
{

class ScreenMain : public Screen
{
  using GUI = gui::ScreenMainGUI;
  GUI * m_gui;
  
  // The game world data & it's rendering data.
  GameWorld *m_world;
  

public:
  ScreenMain(Game * game)
  : Screen(game, "ScreenMain", {0, 0, 0, 255}),
    m_gui{nullptr},
    m_world{nullptr}
  { };

  // Prevent copying
  ScreenMain(const ScreenMain &) = delete;
  ScreenMain &operator=(const ScreenMain &) = delete;

  virtual ~ScreenMain();

  SDL_AppResult init() override;

  SDL_AppResult event(SDL_Event *event) override;
  SDL_AppResult update() override;
  SDL_AppResult render(SDL_Renderer *renderer) override;
};

} // namespace bipsy::gaiasim

#endif // BIPSY_GAIASIM_SCREEN_MAIN_HPP