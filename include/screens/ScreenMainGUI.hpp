/**
 * @file ScreenMainGUI.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief The GUI system of ScreenMain.
 *
 * This gives `GameWorld::render` a 2D bounds to draw their final
 * texture buffer to, that is drawn to the screen within the
 * GUI system during `ScreenMainGUI::render`.
 * Interally, we use a set of `Panel`s to align the GUI into a gridbox of sorts,
 * then dynamically stretch them to the screen size.
 * Currently just rect primitives with a texture rendered from GameWorld.
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
#pragma once
#ifndef BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP
#define BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP


// #region Library Includes

#include "Game.hpp"

#include "SDL3/SDL.h"  // IWYU pragma: keep SDL_FRect, SDL_Color, SDL_Texture...

#include <string>


// #endregion

namespace bipsy::gaiasim
{


// #region Forward Declarations

class GameWorld;

// #endregion


namespace gui
{


/**
 * @brief Holds Panels and the logic for rendering them.
 *
 * Also takes in a GameWorld and calls it's rendering function to
 * render it within a panel.
 *
 */
class ScreenMainGUI
{

  // #region Inner Class `Panel` POD structure

  /**
   * @brief A data structure that holds a partition of the game screen.
   *
   * A percentage partition rectangle of the game screen is where the panel
   * is rendered, given a panel color and top dipslay title.
   *
   */
  struct Panel
  {
    /**
     * @brief values [0, 1]. This is the percentage of the width/height
     *        of the full window that this panel takes up.
     */
    //
    SDL_FRect m_partition;

    /**
     * @brief Display title of the panel, shown at the top-center.
     */
    std::string m_title;

    /**
     * @brief Background rendering color of the panel.
     */
    SDL_Color m_color;

    /**
     * @brief Pointer to the texture of the title text, rendered from a font.
     */
    SDL_Texture * m_title_texture  = nullptr;

    /**
     * @brief Cached dimensions of the title texture, given the default font.
     *
     * Used to determine when we should switch to a smaller font.
     *
     * TODO: This needs to be modified in some way
     *        because it doesn't work on Android
     *
     */
    SDL_FRect m_title_texture_size = {0, 0, 0, 0};

    /**
     * @brief Flag for if small font should be used.
     */
    bool m_use_small_font          = false;
  };

  
  // #endregion
  // #region Private Data Members (`ScreenMainGUI`)

  /**
   * @brief The main game panel that renders the GameWorld's viewport.
   */
  Panel m_main_panel;

  /**
   * @brief The left sidebar panel, displaying some control buttons.
   */
  Panel m_left_sidebar;

  /**
   * @brief The top sidebar panel, displaying some console/information.
   */
  Panel m_top_sidebar;


  /**
   * @brief Cached window width, for panel size calculations.
   */
  int m_window_width;

  /**
   * @brief Cached window height, for panel size calcualations.
   */
  int m_window_height;


  // #endregion

public:

  // #region Constructor & Destructor

  /**
   * @brief Constructs the GUI system for ScreenMain with panels.
   */
  ScreenMainGUI();

  /**
   * @brief Cleans up titlebar text box tetures from panels.
   *
   */
  ~ScreenMainGUI();


  // #endregion
  // #region Public Methods

  /**
   * @brief Determine if window size changed and update panel layout.
   *
   * Determines when the title bar textures of panels are regenerated.
   *
   * @param game Uses rendering and font data (passthrough to other methods)
   * @return SDL_AppResult
   */
  SDL_AppResult update_layout(Game * game);

  /**
   * @brief Renders the panels, and passes the GameWorld for
   *        rendering its viewport within the main panel.
   *
   * @param renderer Renderer to render the panel to
   * @param world The game world to pass to the main panel for rendering.
   * @return SDL_AppResult
   */
  SDL_AppResult render(SDL_Renderer * renderer, GameWorld * world);

  /**
   * @brief React to an SDL event for the GUI (Does nothing currently)
   *
   * @param game The encapsulating game instance.
   * @param event The event we're reacting to.
   * @return SDL_AppResult
   */
  SDL_AppResult event(Game * game, SDL_Event * event);


  // #endregion

private:

  // #region Private Methods

  /**
   * @brief Generates titlebar textures for panels.
   *
   * @param game The input game (for rendering and font data)
   * @return SDL_AppResult
   */
  SDL_AppResult generate_textures(Game * game);

  /**
   * @brief Generates a single titlebar texture for a panel.
   *
   * @param game The input game (for rendering and font data)
   * @param panel The panel we're rendering the title texture for
   * @return SDL_AppResult
   */
  SDL_AppResult create_panel_title_texture(Game * game, Panel & panel);

  /**
   * @brief Render the panel with optional `GameWorld` to render.
   *
   * @param renderer the input renderer to use for rendering.
   * @param panel the panel we're rendering here.
   * @param world the game world to render as well (given the main panel)
   * @return SDL_AppResult
   */
  SDL_AppResult render_panel(SDL_Renderer * renderer,
                             Panel &        panel,
                             GameWorld *    world = nullptr);


  // #endregion

};  // class ScreenMainGUI

}  // namespace gui

}  // namespace bipsy::gaiasim

#endif  // BIPSY_GAIASIM_SCREEN_MAIN_GUI_HPP
