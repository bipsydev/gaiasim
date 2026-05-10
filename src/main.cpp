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


#include "Game.hpp"


// Use SDL callback entrypoints instead of `main` function
#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"

// Include SDL and extension libraries
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"

// STL headers
#include <string>

// Using declarations
using bipsy::gaiasim::Game;
using namespace bipsy::sdlutils;  // log_info, log_error_init, etc


// Helper macro to get typed `Game` object from a void* `appstate` pointer
#define GetGame Game *game = static_cast<Game *>(appstate)


SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{

  log_info("------ SDL_AppInit: Initializing ------");
  // Initialize Game (starts SDL systems and loads initial game state)
  Game *game = new Game();
  log_info("App state initialized successfully");
  
  // Initialize SDL and its subsystems
  if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
  {
    return log_error_init("SDL");
  }
  else
  {
    log_info("SDL3 initialized successfully");
  }

  if (not TTF_Init())
  {
    return log_error_init("SDL_ttf");
  }
  else
  {
    log_info("SDL_ttf initialized successfully");
  }

  if (not MIX_Init())
  {
    return log_error_init("SDL_mixer");
  }
  else
  {
    log_info("SDL_mixer initialized successfully");
  }

  log_info("All subsystems initialized successfully");

  // Create a window
  if (not (game->window = SDL_CreateWindow(
    "gaiasim (SDL3)",
    1280, 720,
    // SDL_WINDOW_VULKAN | 
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN)))
  {
    return log_error_init("SDL_Window *window");
  }
  else
  {
    log_info("SDL_Window created successfully");
  }
  // Center the window on the screen
  // This takes a second, so we wait to show the window
  // until the first frame in SDL_AppIterate
  SDL_SetWindowPosition(game->window,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  // Create a rendering context from a window
  if (not (game->renderer = SDL_CreateRenderer(game->window, NULL)))
  {
    return log_error_init("SDL_Renderer *renderer");
  }
  else
  {
    log_info("SDL_Renderer created successfully");
  }
  // Enable adaptive vsync for the renderer
  SDL_SetRenderVSync(game->renderer, SDL_RENDERER_VSYNC_ADAPTIVE);

  // --- Initialize vertex data ---
  // Polygon 1
  // Triangle 1
  game->polygon1[0].position   = {100, 100};         // top-left vertex
  game->polygon1[0].color      = {1.0, 0, 0, 1.0};   // red
  game->polygon1[1].position   = {300, 100};         // top-right vertex
  game->polygon1[1].color      = {0, 1.0, 0, 1.0};   // green
  game->polygon1[2].position   = {300, 300};         // bottom-right vertex
  game->polygon1[2].color      = {0, 0, 1.0, 1.0};   // blue
  // Triangle 2 (shares top-left and bottom-right vertices with triangle 1)
  game->polygon1[3].position   = {100, 100};         // top-left vertex (same as vertex 0)
  game->polygon1[3].color      = {1.0, 0, 0, 1.0};   // red (same as vertex 0)
  game->polygon1[4].position   = {100, 300};         // bottom-left vertex
  game->polygon1[4].color      = {1.0, 1.0, 0, 1.0}; // yellow 
  game->polygon1[5].position   = {300, 300};         // bottom-right vertex (same as vertex 2)
  game->polygon1[5].color      = {0, 0, 1.0, 1.0};   // blue (same as vertex 2)


#if not __ANDROID__
  // Polygon 2
  // Triangle 1
  game->polygon2[0].position   = {400, 100};         // top-left vertex
  game->polygon2[0].color      = {1.0, 0, 0, 1.0};   // red
  game->polygon2[1].position   = {400, 300};         // bottom-left vertex
  game->polygon2[1].color      = {1.0, 1.0, 0, 1.0}; // yellow
  game->polygon2[2].position   = {600, 100};         // top-right vertex
  game->polygon2[2].color      = {0, 1.0, 0, 1.0};   // green

  // Triangle 2
  game->polygon2[3].position   = {400, 300};         // bottom-left vertex (same as vertex 1)
  game->polygon2[3].color      = {1.0, 1.0, 0, 1.0}; // yellow (same as vertex 1)
  game->polygon2[4].position   = {600, 300};         // bottom-right vertex
  game->polygon2[4].color      = {0, 0, 1.0, 1.0};   // blue
  game->polygon2[5].position   = {600, 100};         // top-right vertex (same as vertex 2)
  game->polygon2[5].color      = {0, 1.0, 0, 1.0};   // green (same as vertex 2
#endif // __ANDROID__



  // Gradient rectangle (4 triangles, 3 vertices each)
  // Triangle 1 (left side)
  game->gradient_rect[0]  = {{100, 100}, {1.0f,0.0f,1.0f,1.0f}, {0.0f,0.0f}}; // top-left vertex (magenta)
  game->gradient_rect[1]  = {{100, 300}, {1.0f,0.0f,0.0f,1.0f}, {0.0f,1.0f}}; // bottom-left vertex (red)
  game->gradient_rect[2]  = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)
  // Triangle 2 (right side)
  game->gradient_rect[3]  = {{300, 100}, {0.0f,1.0f,1.0f,1.0f}, {1.0f,0.0f}}; // top-right vertex (cyan)
  game->gradient_rect[4]  = {{300, 300}, {0.0f,1.0f,0.0f,1.0f}, {1.0f,1.0f}}; // bottom-right vertex (green)
  game->gradient_rect[5]  = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)
  // Triangle 3 (top side)
  game->gradient_rect[6]  = {{100, 100}, {1.0f,0.0f,1.0f,1.0f}, {0.0f,0.0f}}; // top-left vertex (magenta)
  game->gradient_rect[7]  = {{300, 100}, {0.0f,1.0f,1.0f,1.0f}, {1.0f,0.0f}}; // top-right vertex (cyan)
  game->gradient_rect[8]  = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)
  // Triangle 4 (bottom side)
  game->gradient_rect[9]  = {{100, 300}, {1.0f,0.0f,0.0f,1.0f}, {0.0f,1.0f}}; // bottom-left vertex (red)
  game->gradient_rect[10] = {{300, 300}, {0.0f,1.0f,0.0f,1.0f}, {1.0f,1.0f}}; // bottom-right vertex (green)
  game->gradient_rect[11] = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)

  // Move gradient_rect down 300 px
  for (int i = 0; i < Game::GRADIENT_RECT_VERTEX_COUNT; i++)
  {
    game->gradient_rect[i].position.y += 300;
  }

  // Everything loaded correctly, valid game state
  *appstate = game;

  log_info("------ SDL_AppInit: Initialization complete ------");

  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  if (event->type == SDL_EVENT_QUIT)
  {
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

