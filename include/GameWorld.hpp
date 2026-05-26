#pragma once
#ifndef BIPSY_GAIASIM_GAMEWORLD_HPP
#define BIPSY_GAIASIM_GAMEWORLD_HPP


#include "SDL3/SDL.h"

#include <string>


namespace bipsy::gaiasim
{

// Forward declaration of Game class to avoid circular dependency
class Game;

class GameWorld
{
  // Reference to the Game that owns this GameWorld
  Game *m_game;

  // for now, use an ASCII array to display a "map"
  const std::string map = {
    "####################\n"
    "#..................#\n"
    "#...#####..........#\n"
    "#...#...#..........#\n"
    "#...#...#..........#\n"
    "#...#####..........#\n"
    "#..................#\n"
    "#..................#\n"
    "#..................#\n"
    "####################"
  };
  SDL_Texture *map_texture;
  
public:
  // needs to grab Game instance here
  GameWorld(Game *game);

  // Prevent copying (for now) because of pointer data members
  // May implement later if the need arises
  GameWorld(const GameWorld &) = delete;
  GameWorld &operator=(const GameWorld &) = delete;


  SDL_AppResult init();

  SDL_AppResult event(SDL_Event *event);
  SDL_AppResult update();
  SDL_AppResult render(SDL_Renderer *renderer, SDL_FRect *bounds = nullptr);

  ~GameWorld();

private:
  SDL_AppResult generate_map_texture();

}; // class GameWorld


} // namespace bipsy::gaiasim


#endif // BIPSY_GAIASIM_GAMEWORLD_HPP