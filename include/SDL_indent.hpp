#pragma once
#ifndef BIPSY_SDL3UTILS_SDL_LOGINDENT_HPP
#define BIPSY_SDL3UTILS_SDL_LOGINDENT_HPP

#include "tab.hpp"

#include <SDL3/SDL.h>

//inline void SDL_LogIndent(int indent, const char* message)
inline void SDL_LogIndent(int indent, const char* message, ...)
{
  //SDL_Log((bipsy::tab(indent) + message).c_str());

  va_list args_list;

  va_start(args_list, message);
  SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION,
                  SDL_LOG_PRIORITY_INFO,
                  (bipsy::tab(indent) + message).c_str(),
                  args_list);
  va_end(args_list);
}

#endif // BIPSY_SDL3UTILS_SDL_LOGINDENT_HPP