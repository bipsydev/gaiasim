#include "screens/ScreenMain.hpp"

#include <format>


namespace bipsy::gaiasim
{

SDL_AppResult ScreenMain::init()
{
  // Initialize GUI instance for this screen
  gui = new GUI();

  return SDL_APP_CONTINUE;
}

ScreenMain::~ScreenMain()
{
  // Clean up GUI instance
  if (gui != nullptr)
  {
    delete gui;
    gui = nullptr;
  }
}

SDL_AppResult ScreenMain::event(SDL_Event *event)
{
  if (event->type == SDL_EVENT_KEY_DOWN)
  {
    if (event->key.key == SDLK_N ||
        event->key.key == SDLK_ESCAPE)
    {
      log_info(std::format("'{}' key pressed, switching back to test screen...", SDL_GetKeyName(event->key.key)));
      game()->switch_screen(0); // Switch back to the first screen (ScreenTest)
      return SDL_APP_CONTINUE;
    }
  }

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