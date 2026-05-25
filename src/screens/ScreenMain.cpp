#include "screens/ScreenMain.hpp"


namespace bipsy::gaiasim
{

SDL_AppResult ScreenMain::init()
{
  // Initialize GUI instance for this screen
  gui = new GUI();

  //generate the map texture based on the ASCII map
  generate_map_texture();

  return SDL_APP_CONTINUE;
}

void ScreenMain::generate_map_texture()
{
  SDL_Surface *map_surface = TTF_RenderText_Blended_Wrapped(game()->font(), map.c_str(), 0, {255, 255, 255, 255}, 0);
  map_texture = SDL_CreateTextureFromSurface(game()->renderer(), map_surface);
  SDL_DestroySurface(map_surface);
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
      log_info("'%s' key pressed, switching back to test screen...",
        SDL_GetKeyName(event->key.key));
      game()->switch_screen(0); // Switch back to the first screen (ScreenTest)
      return SDL_APP_CONTINUE;
    }
  }
  else if (not SDL_HasKeyboard() && event->type == SDL_EVENT_FINGER_DOWN)
  {
    log_info("Touch event #%lld pressed at (%.4f, %.4f) "
      "with pressure %.4f and type %d",
      static_cast<long long>(event->tfinger.fingerID),
      event->tfinger.x, event->tfinger.y,
      event->tfinger.pressure,
      static_cast<int>(event->tfinger.type));
    game()->switch_screen(0); // Switch back to the first screen (ScreenTest)
    return SDL_APP_CONTINUE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenMain::update()
{
  // update based on game's renderer
  if (SDL_AppResult result = gui->update_layout(game()))
    return result;

  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenMain::render(SDL_Renderer *renderer)
{
  // Render the GUI for this screen
  if (SDL_AppResult result = gui->render(renderer, map_texture))
    return result;

  return SDL_APP_CONTINUE;
}

} // namespace bipsy::gaiasim