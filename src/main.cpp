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


// Use SDL callback entrypoints instead of `main` function
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

// Include SDL and extension libraries
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"

// STL headers
#include <string>


// Helper macro to get typed AppState from a void* `appstate` pointer
#define GetApp AppState *app = static_cast<AppState *>(appstate)


/**
 * @brief Local application running state structure.
 * 
 * Contains pointers to SDL objects for window, renderer, etc along with
 * other local application running state information. This object is
 * initialized once in SDL_AppInit and then passed around in the rest of the
 * SDL callback entrypoints as a `void*` (must be cast into a `Appstate*`).
 */
struct AppState
{
  SDL_Window   *window   = nullptr; // Window object
  SDL_Renderer *renderer = nullptr; // Rendering context to window

  static const int VERTEX_COUNT = 6;
  SDL_Vertex polygon1[VERTEX_COUNT]; // Vertices for gradient rectangle 1
  SDL_Vertex polygon2[VERTEX_COUNT]; // Vertices for gradient rectangle 2

  int frame = 0;      // Frame count
  Uint64 time_ns = 0; // Time (nanoseconds) since SDL initialization
                      // (updated at the beginning of each frame)
};


inline SDL_AppResult log_error_init(const char *subsystem)
{
  SDL_LogError(SDL_LOG_CATEGORY_ERROR,
    "Failed to initialize %s: %s", subsystem, SDL_GetError());
  return SDL_APP_FAILURE;
}

inline void log_info(const char *message)
{
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message);
}


SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  log_info("------ SDL_AppInit: Initializing ------");
  // Initialize app state structure
  AppState *app = new AppState();
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
  if (not (app->window = SDL_CreateWindow(
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
  SDL_SetWindowPosition(app->window,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  // Create a rendering context from a window
  if (not (app->renderer = SDL_CreateRenderer(app->window, NULL)))
  {
    return log_error_init("SDL_Renderer *renderer");
  }
  else
  {
    log_info("SDL_Renderer created successfully");
  }
  // Enable adaptive vsync for the renderer
  SDL_SetRenderVSync(app->renderer, SDL_RENDERER_VSYNC_ADAPTIVE);

  // initialize vertices for gradient rectangle
  // Triangle 1
  app->polygon1[0].position   = {100, 100};         // top-left vertex
  app->polygon1[0].color      = {1.0, 0, 0, 1.0};   // red
  app->polygon1[1].position   = {300, 100};         // top-right vertex
  app->polygon1[1].color      = {0, 1.0, 0, 1.0};   // green
  app->polygon1[2].position   = {300, 300};         // bottom-right vertex
  app->polygon1[2].color      = {0, 0, 1.0, 1.0};   // blue

  // Triangle 2 (shares top-left and bottom-right vertices with triangle 1)
  app->polygon1[3].position   = {100, 100};         // top-left vertex (same as vertex 0)
  app->polygon1[3].color      = {1.0, 0, 0, 1.0};   // red (same as vertex 0)
  app->polygon1[4].position   = {100, 300};         // bottom-left vertex
  app->polygon1[4].color      = {1.0, 1.0, 0, 1.0}; // yellow 
  app->polygon1[5].position   = {300, 300};         // bottom-right vertex (same as vertex 2)
  app->polygon1[5].color      = {0, 0, 1.0, 1.0};   // blue (same as vertex 2)


  // Polygon 2 (gradient rectangle with bottom-left to top-right seam across the diagonal)
  app->polygon2[0].position   = {400, 100};         // top-left vertex
  app->polygon2[0].color      = {1.0, 0, 0, 1.0};   // red
  app->polygon2[1].position   = {400, 300};         // bottom-left vertex
  app->polygon2[1].color      = {1.0, 1.0, 0, 1.0}; // yellow
  app->polygon2[2].position   = {600, 100};         // top-right vertex
  app->polygon2[2].color      = {0, 1.0, 0, 1.0};   // green

  // Triangle 2
  app->polygon2[3].position   = {400, 300};         // bottom-left vertex (same as vertex 1)
  app->polygon2[3].color      = {1.0, 1.0, 0, 1.0}; // yellow (same as vertex 1)
  app->polygon2[4].position   = {600, 300};         // bottom-right vertex
  app->polygon2[4].color      = {0, 0, 1.0, 1.0};   // blue
  app->polygon2[5].position   = {600, 100};         // top-right vertex (same as vertex 2)
  app->polygon2[5].color      = {0, 1.0, 0, 1.0};   // green (same as vertex 2

  /*
  app->vertices[0].position.x = 400;
  app->vertices[0].position.y = 150;
  app->vertices[0].color.r    = 1.0f;
  app->vertices[0].color.g    = 0.0f;
  app->vertices[0].color.b    = 0.0f;
  app->vertices[0].color.a    = 1.0f;

  app->vertices[1].position.x = 200;
  app->vertices[1].position.y = 450;
  app->vertices[1].color.r    = 0.0f;
  app->vertices[1].color.g    = 1.0f;
  app->vertices[1].color.b    = 0.0f;
  app->vertices[1].color.a    = 1.0f;

  app->vertices[2].position.x = 600;
  app->vertices[2].position.y = 450;
  app->vertices[2].color.r    = 0.0f;
  app->vertices[2].color.g    = 0.0f;
  app->vertices[2].color.b    = 1.0f;
  app->vertices[2].color.a    = 1.0f;

  app->vertices[3].position.x = 800;
  app->vertices[3].position.y = 150;
  app->vertices[3].color.r    = 1.0f;
  app->vertices[3].color.g    = 1.0f;
  app->vertices[3].color.b    = 0.0f;
  app->vertices[3].color.a    = 1.0f;
  */

  // Everything loaded correctly, valid app state
  *appstate = app;

  log_info("------ SDL_AppInit: Initialization complete ------");

  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  if (event->type == SDL_EVENT_QUIT)
  {
    return SDL_APP_SUCCESS; // Request app termination on quit event
  }

  return SDL_APP_CONTINUE; // Continue processing events
}


SDL_AppResult draw(AppState *app);


SDL_AppResult SDL_AppIterate(void *appstate)
{
  GetApp; // Get typed `app` from `appstate` void pointer

  // update frame time
  app->time_ns = SDL_GetTicksNS();

  // log frame count (with padding)
  auto frame_count = std::to_string(app->frame);
  frame_count.resize(5, ' '); // Pad frame count to 5 characters for better readability
  log_info(("--- frame: " + frame_count + " ---").c_str());
  log_info(("Frame time (s): " + std::to_string(app->time_ns / 1000000000.0)).c_str());
  
  // Make window visible on first frame (after initialization)
  if (app->frame == 0)
  {
    SDL_ShowWindow(app->window);
    log_info("Window shown");
  }

  // draw the frame, and return anything but SDL_APP_CONTINUE
  if (SDL_AppResult result = draw(app))
  {
    return result; // Return failure result if drawing failed
  }

  // increment frame count and return (continue running to next callback)
  app->frame++;
  return SDL_APP_CONTINUE;
}


SDL_AppResult draw(AppState *app)
{
  // Clear the screen with a solid color
  SDL_SetRenderDrawColor(app->renderer, 0, 128, 255, 255);
  SDL_RenderClear(app->renderer);
  // --- Drawing block ---
  {
    // draw gradient rectangle
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    if (not SDL_RenderGeometry(app->renderer, NULL,
      app->polygon1
      // (SDL_Vertex[]){
      //   {{100, 100}, {1,0,0,1}, {0,0}}, // top-left vertex (red)
      //   {{300, 100}, {0,1,0,1}, {1,0}}, // top-right vertex (green)
      //   {{300, 300}, {0,0,1,1}, {1,1}}, // bottom-right vertex (blue)
      //   {{100, 300}, {1,1,0,1}, {0,1}}  // bottom-left vertex (yellow)
      // }
      ,app->VERTEX_COUNT,
    NULL, 0)
    )
    {
      SDL_Log("num_verticies: app->VERTEX_COUNT = %d", app->VERTEX_COUNT);
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
        "Failed to render geometry: %s", SDL_GetError());
      return SDL_APP_FAILURE; // Return failure result if rendering failed
    }

    if (not SDL_RenderGeometry(app->renderer, NULL,
      app->polygon2 ,app->VERTEX_COUNT, NULL, 0))
    {
      SDL_Log("num_verticies: app->VERTEX_COUNT = %d", app->VERTEX_COUNT);
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
        "Failed to render geometry: %s", SDL_GetError());
      return SDL_APP_FAILURE; // Return failure result if rendering failed
    }
  }
  // Present the rendered frame to the screen
  SDL_RenderPresent(app->renderer);

  return SDL_APP_CONTINUE; // Continue running the app
}


void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  GetApp;

  // Deallocate window and renderer
  SDL_DestroyWindow(app->window);
  SDL_DestroyRenderer(app->renderer);

  // Deinitialize SDL and its subsystems
  MIX_Quit();
  TTF_Quit();
  SDL_Quit();

  // Deallocate app state
  delete app;
}

