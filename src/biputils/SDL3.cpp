#include "biputils/SDL3.hpp"


#include "biputils/to_string.hpp"

#include "SDL3/SDL.h"  // IWYU pragma: keep SDL_AppResult


namespace bipsy
{


template <>
std::string_view to_string<SDL_AppResult>(SDL_AppResult result)
{
  switch (result)
  {
  case SDL_APP_CONTINUE: return "SDL_APP_CONTINUE";
  case SDL_APP_SUCCESS:  return "SDL_APP_SUCCESS";
  case SDL_APP_FAILURE:  return "SDL_APP_FAILURE";
  default:               return "<Unknown?>";
  }
}


}  // namespace bipsy
