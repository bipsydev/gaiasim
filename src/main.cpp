/**
 * @file main.cpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Main entry point for SDL3 application
 * @version 0.1
 * @date 2026-05-06
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



/*******************************************************************************
 *  LIBRARY INCLUDES/IMPORTS                                                   *
 ******************************************************************************/

// --- Project Headers ---
#include "Game.hpp"   // bipsy::gaiasim::Game


// --- Library (SDL3) Headers ---
// Use SDL callback entrypoints instead of `main` function
#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"

// Include SDL and extension libraries
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"


// --- STL Headers ---
#include <string>   // std::to_string


// Using declarations
using bipsy::gaiasim::Game;
using namespace bipsy::sdlutils;  // log_info, log_error_init, etc


// Helper macro to get typed `Game` object from a void* `appstate` pointer
#define GetGame Game *game = static_cast<Game *>(appstate)


SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  log_info("------ AppInit: Initializing ------");
  // Initialize Game (starts SDL systems and loads initial game state)
  Game *game = new Game();

  // Everything loaded correctly, valid game state
  *appstate = game;

  log_info("------ AppInit: Initialization complete ------");

  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  GetGame;
  if (SDL_AppResult result = game->event(event))
  {
    return result;
  }

  return SDL_APP_CONTINUE; // Continue processing events, or next callback
}


SDL_AppResult SDL_AppIterate(void *appstate)
{
  GetGame; // Get typed `game` from `appstate` void pointer
  if (SDL_AppResult result = game->iterate())
  {
    return result;
  }

  return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  GetGame;

  // Deallocate window and renderer
  SDL_DestroyWindow(game->window);
  SDL_DestroyRenderer(game->renderer);

  // Deinitialize SDL and its subsystems
  MIX_Quit();
  TTF_Quit();
  SDL_Quit();

  // Deallocate game state
  delete game;
}

