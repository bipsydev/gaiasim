#include "screens/ScreenMain.hpp"

namespace bipsy::gaiasim
{

SDL_AppResult ScreenMain::init()
{
  // Initialize GUI instance for this screen
  gui = new GUI();

  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenMain::event(SDL_Event *event)
{
  // TODO implement event handling logic for this screen here

  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenMain::update()
{
  // TODO implement update logic for this screen here

  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenMain::render(SDL_Renderer *renderer)
{
  // Render the GUI for this screen
  // TODO implement GUI rendering logic here

  return SDL_APP_CONTINUE;
}

} // namespace bipsy::gaiasim