#include "ScreenTest.hpp"

namespace bipsy::gaiasim
{
  

ScreenTest::ScreenTest()
{
}

ScreenTest::~ScreenTest()
{
}

SDL_AppResult ScreenTest::init()
{
  return SDL_APP_CONTINUE;
}

void ScreenTest::deinit()
{
}

SDL_AppResult ScreenTest::event(SDL_Event *event)
{
  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::update()
{
  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::render()
{
  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::post_render_update()
{
  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::show()
{
  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::hide()
{
  return SDL_APP_CONTINUE;
}

} // namespace bipsy::gaiasim
