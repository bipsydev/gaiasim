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
  if (event->type == SDL_EVENT_QUIT)
  {
    log_info("Quit event received, terminating...");
    return SDL_APP_SUCCESS; // Request game termination on quit event
  }

  return SDL_APP_CONTINUE; // Continue processing events
}


SDL_AppResult draw(Game *game);


SDL_AppResult SDL_AppIterate(void *appstate)
{
  GetGame; // Get typed `game` from `appstate` void pointer

  // update frame time
  game->time_ns = SDL_GetTicksNS();

  // log frame count (with padding)
  auto frame_count = std::to_string(game->frame);
  frame_count.resize(5, ' '); // Pad frame count to 5 characters for better readability
  log_info(("--- frame: " + frame_count + " ---").c_str());
  log_info(("Frame time (s): " + std::to_string(game->time_ns / 1000000000.0)).c_str());
  
  // Make window visible on first frame (after initialization)
  if (game->frame == 0)
  {
    SDL_ShowWindow(game->window);
    log_info("Window shown");
  }

  // draw the frame, and return anything but SDL_APP_CONTINUE
  if (SDL_AppResult result = draw(game))
  {
    return result; // Return failure result if drawing failed
  }

  // increment frame count and return (continue running to next callback)
  game->frame++;
  return SDL_APP_CONTINUE;
}


SDL_AppResult draw(Game *game)
{
  // Clear the screen with a solid color
  SDL_SetRenderDrawColor(game->renderer, 0, 128, 255, 255);
  SDL_RenderClear(game->renderer);
  // --- Drawing block ---
  {
    // draw gradient rectangle
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    if (not SDL_RenderGeometry(game->renderer, NULL,
      game->polygon1
      // (SDL_Vertex[]){
      //   {{100, 100}, {1,0,0,1}, {0,0}}, // top-left vertex (red)
      //   {{300, 100}, {0,1,0,1}, {1,0}}, // top-right vertex (green)
      //   {{300, 300}, {0,0,1,1}, {1,1}}, // bottom-right vertex (blue)
      //   {{100, 300}, {1,1,0,1}, {0,1}}  // bottom-left vertex (yellow)
      // }
      ,game->VERTEX_COUNT,
    NULL, 0)
    )
    {
      SDL_Log("num_verticies: game->VERTEX_COUNT = %d", game->VERTEX_COUNT);
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
        "Failed to render geometry polygon1: %s", SDL_GetError());
      return SDL_APP_FAILURE; // Return failure result if rendering failed
    }

#if not __ANDROID__
    if (not SDL_RenderGeometry(game->renderer, NULL,
      game->polygon2 ,game->VERTEX_COUNT, NULL, 0))
    {
      SDL_Log("num_verticies: game->VERTEX_COUNT = %d", game->VERTEX_COUNT);
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
        "Failed to render geometry polygon2: %s", SDL_GetError());
      return SDL_APP_FAILURE; // Return failure result if rendering failed
    }
#endif // __ANDROID__

    if (not SDL_RenderGeometry(game->renderer, NULL,
      game->gradient_rect ,game->GRADIENT_RECT_VERTEX_COUNT, NULL, 0))
    {
      SDL_Log("num_verticies: game->GRADIENT_RECT_VERTEX_COUNT = %d", game->GRADIENT_RECT_VERTEX_COUNT);
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
        "Failed to render geometry gradient_rect: %s", SDL_GetError());
      return SDL_APP_FAILURE; // Return failure result if rendering failed
    }
  }
  // Present the rendered frame to the screen
  SDL_RenderPresent(game->renderer);

  return SDL_APP_CONTINUE; // Continue running the game
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

