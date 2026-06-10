/**
 * @file ScreenMain.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief This is the "Main" game screen, which is what actively takes
 *        control of the application from SDL when active in `Game`.
 *
 * This delegates GUI rendering to `ScreenMainGUI.hpp`.
 * This controls a `GameWorld` object and feeds it to `ScreenMainGUI` for
 * rendering to the screen within the GUI system.
 *
 * @version a_0.0.1-pre
 * @date 2026-05-17
 *
 * @copyright Copyright (c) 2026
 *
 * @section License
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 **/
#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_HPP


#include "Screen.hpp"
#include "ScreenMainGUI.hpp"
#include "Game.hpp"
#include "GameWorld.hpp"

#include "SDL3/SDL.h"  // IWYU pragma: keep SDL_AppResult

#include <string>


namespace bipsy::gaiasim
{

class ScreenMain : public Screen
{
  using GUI = gui::ScreenMainGUI;
  GUI * m_gui;

  // The game world data & it's rendering data.
  GameWorld * m_world;


public:
  ScreenMain(Game * game)
  : Screen(game, "ScreenMain", {0, 0, 0, 255}), m_gui{nullptr}, m_world{nullptr}
  {}

  // Prevent copying
  ScreenMain(const ScreenMain &)              = delete;
  ScreenMain & operator =(const ScreenMain &) = delete;

  virtual ~ScreenMain();

  SDL_AppResult init() override;

  SDL_AppResult event(SDL_Event * event) override;
  SDL_AppResult update() override;
  SDL_AppResult render(SDL_Renderer * renderer) override;


};  // class ScreenMain


}  // namespace bipsy::gaiasim


#endif  // BIPSY_GAIASIM_SCREEN_MAIN_HPP
