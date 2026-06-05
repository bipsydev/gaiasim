#include "screens/ScreenMain.hpp"


namespace bipsy::gaiasim
{



SDL_AppResult ScreenMain::init()
{
  // Initialize GUI instance for this screen
  gui = new GUI();

  // init the game world
  world = new GameWorld(game());

  // attempt to generate the map texture based on the ASCII map
  if (SDL_AppResult result = world->init())
  {
    // if we failed, deallocate what we allocated earlier before returning
    delete gui;
    gui = nullptr;

    delete world;
    world = nullptr;
    
    return result;
  }

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

  // Clean up game world
  if (world != nullptr)
  {
    delete world;
    world = nullptr;
  }
}



SDL_AppResult ScreenMain::event(SDL_Event *event)
{
  // Screen-switching (N-key or tap on touch device)
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


  // handle GUI events first
  // TODO system to remove events from further processing (marked as "handled")
  SDL_AppResult result;
  if ( (result = gui->event(game(), event)) )
    return result;
  
  // handle game world events next
  if ( (result = world->event(event)) )
    return result;


  return SDL_APP_CONTINUE;
}



SDL_AppResult ScreenMain::update()
{
  SDL_AppResult result;

  // update the game world
  if ( (result = world->update()) )
    return result;

  // update based on game's renderer
  if ( (result = gui->update_layout(game())) )
    return result;

  return SDL_APP_CONTINUE;
}



SDL_AppResult ScreenMain::render(SDL_Renderer *renderer)
{
  SDL_AppResult result;
  // Render the GUI for this screen (includes rendering of game world texture)
  if ( (result = gui->render(renderer, world)) )
    return result;

  return SDL_APP_CONTINUE;
}



} // namespace bipsy::gaiasim
