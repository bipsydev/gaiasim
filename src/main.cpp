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
 *  LIBRARY INCLUDES                                                           *
 ******************************************************************************/

// --- Project Headers ---
#include "Game.hpp"   // bipsy::gaiasim::Game class


// --- Library (SDL3) Headers ---
// Tell SDL  to use its callback entrypoints instead of a `main` function:
#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"

// Include SDL and extension libraries (C APIs)
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"



/*******************************************************************************
 *  USING DECLARATIONS                                                         *
 ******************************************************************************/

using bipsy::gaiasim::Game,
      bipsy::sdlutils::log_info;



/*******************************************************************************
 *  MACRO DEFINITIONS                                                          *
 ******************************************************************************/

 // Helper macro to get typed `Game` object from a void* `appstate` pointer
#define  GetGame  Game *game = static_cast<Game *>(appstate)


/*******************************************************************************
 *  SDL CALLBACK ENTRYPOINTS                                                   *
 *-----------------------------------------------------------------------------*
 * Instead of a `main` function, we've told SDL we want to use it's callback   *
 * system instead, so we define the following functions instead as application *
 * entrypoints. SDL calls these at the best time for each operating system,    *
 * so we don't need to write separate entrypoint logic for each.               *
 ******************************************************************************/


/**
 * @brief Entrypoint for SDL application initialization.
 * 
 * This is called by SDL when the application starts automatically because
 * we #defined `SDL_MAIN_USE_CALLBACKS`.
 * 
 * Simply initializes the `Game` object and passes it back to SDL
 * as the `appstate` pointer (intended to be passed around in
 * the rest of the callbacks as a global app state).
 * 
 * @param appstate We set this to a `Game *` that we allocate in this function.
 *                 SDL passes this to the other callbacks as a `void *` pointer.
 *                 Our `GetGame` macro can be used to cast it back to `Game *`.
 * @param argc The classic `argc` argument from `main()`, passed by SDL.
 * @param argv The classic `argv` argument from `main()`, passed by SDL.

 * @return SDL_AppResult `SDL_APP_CONTINUE` to continue running,
 *                       `SDL_APP_SUCCESS` to terminate the app successfully,
 *                       `SDL_APP_FAILURE` to terminate the app with an error.
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  log_info("------ AppInit: Initializing ------");
  // Initialize Game (starts SDL systems and loads initial game state)
  *appstate = new Game();
  log_info("------ AppInit: Initialization complete ------");

  return SDL_APP_CONTINUE;
}



/**
 * @brief Entrypoint for SDL application event handling.
 * 
 * This is called by SDL when any event occurs, such as keyboard input,
 * mouse movement, window events, etc.
 * 
 * Here, we simply forward the event to the `Game`'s event handler,
 * which will handle it based on the current active screen and game state.
 * 
 * @param appstate This is the `Game *` that we allocated in `SDL_AppInit`.
 *                 The `GetGame` macro is used to cast this back to a `Game *`.
 * @param event The SDL_Event that occurred, passed by SDL.
 * 
 * @return SDL_AppResult `SDL_APP_CONTINUE` to continue running,
 *                       `SDL_APP_SUCCESS` to terminate the app successfully,
 *                       `SDL_APP_FAILURE` to terminate the app with an error.
 */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  GetGame;  // Use our macro to turn this back into a `Game *`
  return game->event(event);
}



/**
 * @brief Entrypoint for SDL application main loop/frame iteration.
 * 
 * This is called by SDL every time the game needs to update, or in other words,
 * once per frame. This takes care of the state update logic along with
 * drawing to the frame buffer and presenting it to the screen as a frame.
 * 
 * @param appstate This is the `Game *` that we allocated in `SDL_AppInit`.
 *                 The `GetGame` macro is used to cast this back to a `Game *`.
 * 
 * @return SDL_AppResult `SDL_APP_CONTINUE` to continue running,
 *                       `SDL_APP_SUCCESS` to terminate the app successfully,
 *                       `SDL_APP_FAILURE` to terminate the app with an error.
 */
SDL_AppResult SDL_AppIterate(void *appstate)
{
  GetGame;
  return game->iterate();
}



/**
 * @brief Entrypoint for SDL application quit/termination.
 * 
 * This is called by SDL when the application is quitting, either from a quit
 * event or from an error. Here, we clean up the `Game` state and deinitialize
 * SDL and its subsystems before the application terminates.
 * 
 * @param appstate This is the `Game *` that we allocated in `SDL_AppInit`.
 *                 The `GetGame` macro is used to cast this back to a `Game *`.
 *                 We only need to deallocate the pointer here.
 * @param result The `SDL_AppResult` value that indicates whether the app
 *               is quitting successfully or with an error.
 */
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

