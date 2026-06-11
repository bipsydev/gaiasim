/**
 * @file ScreenMain.cpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Implementation for `ScreenMain` lifecycle events.
 *
 * Essentially just delegates GUI logic to `ScreenMainGUI` and
 * the actual game logic to `GameWorld`.
 *
 * @version a_0.0.1-pre
 * @date 2026-05-23
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
#include "screens/ScreenMain.hpp"


#include "screens/ScreenMainGUI.hpp"
#include "GameWorld.hpp"
#include "biputils/SDL3.hpp"

#include "SDL3/SDL.h"  // IWYU pragma: keep SDL_AppResult, SDL_Event, SDLK_N...


using bipsy::sdl3_utils::Log;


namespace bipsy::gaiasim
{


SDL_AppResult ScreenMain::init()
{
  LOG_FRAME_CLASS(ScreenMain);

  // Initialize GUI instance for this screen
  m_gui   = new GUI();

  // init the game world
  m_world = new GameWorld(game());

  // attempt to generate the map texture based on the ASCII map
  if (SDL_AppResult result = m_world->init())
  {
    // if we failed, deallocate what we allocated earlier before returning
    delete m_gui;
    m_gui = nullptr;

    delete m_world;
    m_world = nullptr;

    return result;
  }

  return SDL_APP_CONTINUE;
}


ScreenMain::~ScreenMain()
{
  LOG_FRAME_CLASS(ScreenMain);
  // Clean up GUI instance
  if (m_gui != nullptr)
  {
    delete m_gui;
    m_gui = nullptr;
  }

  // Clean up game world
  if (m_world != nullptr)
  {
    delete m_world;
    m_world = nullptr;
  }
}


SDL_AppResult ScreenMain::event(SDL_Event * event)
{
  // Screen-switching (N-key or tap on touch device)
  if (event->type == SDL_EVENT_KEY_DOWN)
  {
    if (event->key.key == SDLK_N || event->key.key == SDLK_ESCAPE)
    {
      LOG_FRAME_CLASS(ScreenMain);
      Log::info("'{}' key pressed, switching back to test screen...",
                SDL_GetKeyName(event->key.key));
      game()->switch_screen(0);  // Switch back to the first screen (ScreenTest)
      return SDL_APP_CONTINUE;
    }
  }
  else if (not SDL_HasKeyboard() && event->type == SDL_EVENT_FINGER_DOWN)
  {
    LOG_FRAME_CLASS(ScreenMain);
    Log::verbose("Touch event #{} pressed at ({:.4f}, {:.4f}) "
                 "with pressure {:.4f} and type {}",
                 static_cast<long long>(event->tfinger.fingerID),
                 event->tfinger.x,
                 event->tfinger.y,
                 event->tfinger.pressure,
                 static_cast<int>(event->tfinger.type));
    Log::info("Touch event received, switching back to test screen...");
    game()->switch_screen(0);  // Switch back to the first screen (ScreenTest)
    return SDL_APP_CONTINUE;
  }

  // handle GUI events first
  // TODO system to remove events from further processing (marked as "handled")
  SDL_AppResult result;
  if ((result = m_gui->event(game(), event))) return result;

  // handle game world events next
  if ((result = m_world->event(event))) return result;


  return SDL_APP_CONTINUE;
}


SDL_AppResult ScreenMain::update()
{
  LOG_FRAME_CLASS(ScreenMain);

  SDL_AppResult result;

  // update the game world
  if ((result = m_world->update())) return result;

  // update based on game's renderer
  if ((result = m_gui->update_layout(game()))) return result;

  return SDL_APP_CONTINUE;
}


SDL_AppResult ScreenMain::render(SDL_Renderer * renderer)
{
  LOG_FRAME_CLASS(ScreenMain);

  SDL_AppResult result;
  // Render the GUI for this screen (includes rendering of game world texture)
  if ((result = m_gui->render(renderer, m_world))) return result;

  return SDL_APP_CONTINUE;
}


}  // namespace bipsy::gaiasim
