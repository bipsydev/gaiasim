/**
 * @file GameWorld.hpp
 * @author bipsydev (bipsydev@gmail.com)
 * @brief Holds logical game world and state information.
 *
 * This manages a `WorldMap` and runs entities in an ECS system via EnTT
 * to hold and manage the game world state. This contains methods that are 
 * relevant to initialization, reacting to SDL's events, updating game logic,
 * and how the final output frame buffer should be rendered after updating.
 * This class likely routes input events to entities that are listening to them.
 *
 * @version a_0.0.1-pre
 * @date 2026-05-26
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
#ifndef BIPSY_GAIASIM_GAMEWORLD_HPP
#define BIPSY_GAIASIM_GAMEWORLD_HPP


#include "WorldMap.hpp"

#include "SDL3/SDL.h"  // IWYU pragma: keep SDL_Texture, Uint8

#include <string>


namespace bipsy::gaiasim
{

// Forward declaration of Game class to avoid circular dependency
class Game;

class GameWorld
{
  // Reference to the Game that owns this GameWorld
  Game * m_game;

  // for now, use an ASCII array to display a "map"
  // 20 x 10
  std::string   m_map_str = {"####################\n"
                             "#..................#\n"
                             "#.@.#####..........#\n"
                             "#...#...#..........#\n"
                             "#...#...#..........#\n"
                             "#...#####..........#\n"
                             "#..................#\n"
                             "#..................#\n"
                             "#..................#\n"
                             "####################"};
  WorldMap      m_world_map;
  SDL_Texture * m_map_texture;

  Uint8         m_map_width  = 20;
  Uint8         m_map_height = 10;

  // basic player state, TODO implement full player ECS class
  Uint8 m_player_x           = 2;
  Uint8 m_player_y           = 2;
  Uint8 m_player_x_old       = m_player_x;
  Uint8 m_player_y_old       = m_player_y;
  bool  m_moved              = false;
  char  m_player_char        = '@';

public:
  // needs to grab Game instance here
  GameWorld(Game * game);

  // Prevent copying (for now) because of pointer data members
  // May implement later if the need arises
  GameWorld(const GameWorld &)                = delete;
  GameWorld &   operator =(const GameWorld &) = delete;

  SDL_AppResult init();

  SDL_AppResult event(SDL_Event * event);
  SDL_AppResult update();
  SDL_AppResult render(SDL_Renderer * renderer, SDL_FRect * bounds = nullptr);

  ~GameWorld();

private:
  SDL_AppResult generate_map_texture();
  SDL_AppResult generate_world_map();

  SDL_AppResult test_world_map_operations();

};  // class GameWorld

}  // namespace bipsy::gaiasim

#endif  // BIPSY_GAIASIM_GAMEWORLD_HPP
