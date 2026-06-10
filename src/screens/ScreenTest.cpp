#include "screens/ScreenTest.hpp"


#include "Game.hpp"  // log_info
#include "screens/ScreenMain.hpp"
#include "SDL3_utils.hpp"

#include "SDL3/SDL.h"  // IWYU pragma: keep SDL_AppResult, SDL_Surface, SDL_Event...

#include <cstdlib>  // rand
#include <format>   // std::format

namespace bipsy::gaiasim
{

using bipsy::sdl3_utils::Log;

SDL_AppResult ScreenTest::init()
{
  LOG_FRAME_CLASS(ScreenTest);

  // --- Initialize vertex data ---
  Log::verbose("Initializing vertex data for polygons:");
  // Polygon 1
  Log::verbose(Log::indent() + 1, "Initializing vertex data for m_polygon1...");
  // Triangle 1
  m_polygon1[0].position = {100, 100};        // top-left vertex
  m_polygon1[0].color    = {1.0, 0, 0, 1.0};  // red
  m_polygon1[1].position = {300, 100};        // top-right vertex
  m_polygon1[1].color    = {0, 1.0, 0, 1.0};  // green
  m_polygon1[2].position = {300, 300};        // bottom-right vertex
  m_polygon1[2].color    = {0, 0, 1.0, 1.0};  // blue
  // Triangle 2 (shares top-left and bottom-right vertices with triangle 1)
  m_polygon1[3].position = {100, 100};  // top-left vertex (same as vertex 0)
  m_polygon1[3].color    = {1.0, 0, 0, 1.0};    // red (same as vertex 0)
  m_polygon1[4].position = {100, 300};          // bottom-left vertex
  m_polygon1[4].color    = {1.0, 1.0, 0, 1.0};  // yellow
  m_polygon1[5].position
          = {300, 300};  // bottom-right vertex (same as vertex 2)
  m_polygon1[5].color = {0, 0, 1.0, 1.0};  // blue (same as vertex 2)

  // Polygon 2
  Log::verbose(Log::indent() + 1,
               "Success! Initializing vertex data for m_polygon2...");
  // Triangle 1
  m_polygon2[0].position = {400, 100};          // top-left vertex
  m_polygon2[0].color    = {1.0, 0, 0, 1.0};    // red
  m_polygon2[1].position = {400, 300};          // bottom-left vertex
  m_polygon2[1].color    = {1.0, 1.0, 0, 1.0};  // yellow
  m_polygon2[2].position = {600, 100};          // top-right vertex
  m_polygon2[2].color    = {0, 1.0, 0, 1.0};    // green

  // Triangle 2
  m_polygon2[3].position = {400, 300};  // bottom-left vertex (same as vertex 1)
  m_polygon2[3].color    = {1.0, 1.0, 0, 1.0};  // yellow (same as vertex 1)
  m_polygon2[4].position = {600, 300};          // bottom-right vertex
  m_polygon2[4].color    = {0, 0, 1.0, 1.0};    // blue
  m_polygon2[5].position = {600, 100};  // top-right vertex (same as vertex 2)
  m_polygon2[5].color    = {0, 1.0, 0, 1.0};  // green (same as vertex 2


  // Gradient rectangle (4 triangles, 3 vertices each)
  Log::verbose(Log::indent() + 1,
               "Success! Initializing vertex data for m_gradient_rect...");
  // Triangle 1 (left side)
  m_gradient_rect[0] = {
      {100, 100},
      {1.0f, 0.0f, 1.0f, 1.0f},
      {0.0f, 0.0f}
  };  // top-left vertex (magenta)
  m_gradient_rect[1] = {
      {100, 300},
      {1.0f, 0.0f, 0.0f, 1.0f},
      {0.0f, 1.0f}
  };  // bottom-left vertex (red)
  m_gradient_rect[2] = {
      {200, 200},
      {0.5f, 0.5f, 0.5f, 1.0f},
      {0.5f, 0.5f}
  };  // center vertex (gray)
  // Triangle 2 (right side)
  m_gradient_rect[3] = {
      {300, 100},
      {0.0f, 1.0f, 1.0f, 1.0f},
      {1.0f, 0.0f}
  };  // top-right vertex (cyan)
  m_gradient_rect[4] = {
      {300, 300},
      {0.0f, 1.0f, 0.0f, 1.0f},
      {1.0f, 1.0f}
  };  // bottom-right vertex (green)
  m_gradient_rect[5] = {
      {200, 200},
      {0.5f, 0.5f, 0.5f, 1.0f},
      {0.5f, 0.5f}
  };  // center vertex (gray)
  // Triangle 3 (top side)
  m_gradient_rect[6] = {
      {100, 100},
      {1.0f, 0.0f, 1.0f, 1.0f},
      {0.0f, 0.0f}
  };  // top-left vertex (magenta)
  m_gradient_rect[7] = {
      {300, 100},
      {0.0f, 1.0f, 1.0f, 1.0f},
      {1.0f, 0.0f}
  };  // top-right vertex (cyan)
  m_gradient_rect[8] = {
      {200, 200},
      {0.5f, 0.5f, 0.5f, 1.0f},
      {0.5f, 0.5f}
  };  // center vertex (gray)
  // Triangle 4 (bottom side)
  m_gradient_rect[9] = {
      {100, 300},
      {1.0f, 0.0f, 0.0f, 1.0f},
      {0.0f, 1.0f}
  };  // bottom-left vertex (red)
  m_gradient_rect[10] = {
      {300, 300},
      {0.0f, 1.0f, 0.0f, 1.0f},
      {1.0f, 1.0f}
  };  // bottom-right vertex (green)
  m_gradient_rect[11] = {
      {200, 200},
      {0.5f, 0.5f, 0.5f, 1.0f},
      {0.5f, 0.5f}
  };  // center vertex (gray)

  Log::verbose(Log::indent() + 1, "Moving m_gradient_rect down 300 px...");
  // Move m_gradient_rect down 300 px
  for (int i = 0; i < GRADIENT_RECT_VERTEX_COUNT; i++)
    m_gradient_rect[i].position.y += 300;


  // Rainbow triangle vertex data
  Log::verbose(Log::indent() + 1,
               "Success! Initializing vertex data for m_rainbow_triangle...");
  // Top left vertex
  m_rainbow_triangle[0] = {
      {700, 400},
      {1.0f, 0.0f, 0.0f, 1.0f},
      {0.0f, 0.0f}
  };
  // Top right vertex
  m_rainbow_triangle[1] = {
      {1'100, 400},
      {0.0f, 1.0f, 0.0f, 1.0f},
      {1.0f, 0.0f}
  };
  // Bottom vertex
  m_rainbow_triangle[2] = {
      {900, 800},
      {0.0f, 0.0f, 1.0f, 1.0f},
      {0.5f, 1.0f}
  };


  Log::verbose("Vertex data initialized successfully!");

  Log::verbose("Initializing texture for on-screen rendered text from font:");
  if (auto result = init_text_texture())
  {
    Log::error(
            "Error occurred while initializing text texture, terminating..."
    );
    return result;
  }


  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::init_text_texture()
{
  LOG_FRAME_CLASS(ScreenTest);
  Log::verbose("{} texture for rendered text...",
               m_text_texture == nullptr ? "Initializing" : "Updating");

  // Update text string first
  std::string new_text_str = (
          // Message box:
          std::format("System: {}\n", m_system_str) +
          // "FPS: " + std::to_string(game()->fps) + "\n" +
          std::format("Game Time: {:.2f} s\n", game()->time_ns() / 1000000000.0)
          + std::format("Delta Time: {:.4f} ms\n",
                        game()->delta_time_ns() / 1000000.0)
          + std::format("FPS: {:.2f}\n", game()->fps())
          + std::format("Active Screen: {}\n", name())  //+ "\n" +
          // "Number of Running Screens: " +
          // std::to_string(game()->get_num_screens())
  );

  // Check if texture update is necessary (new text generated this frame)
  if (m_text_str != new_text_str)
  {
    Log::verbose("Text content updated, regenerating text texture...");
    m_text_str = new_text_str;
  }
  else
  {
    Log::verbose("Text content unchanged, skipping unnecessary texture update");
    return SDL_APP_CONTINUE;  // No need to update texture if text hasn't
                              // changed
  }

  // Create a surface with rendered text using the loaded font
  // This loads the image data into an `SDL_Surface` in RAM using the CPU.
  SDL_Surface * text_surface = TTF_RenderText_Blended_Wrapped(
          game()->font(),
          m_text_str.c_str(),
          0,
          {255, 255, 255, 255},  // white color
          0                      // Wrap length is 0, so only wrap
                                 // on newline characters
  );
  if (text_surface == nullptr)
    return Log::error_init("text_surface");
  else
    Log::verbose(Log::indent() + 1, "Rendered text surface successfully");

  // If previous texture exists, destroy it to free up GPU memory
  if (m_text_texture != nullptr)
  {
    SDL_DestroyTexture(m_text_texture);
    m_text_texture = nullptr;
    Log::verbose(Log::indent() + 1, "Destroyed previous text texture");
  }

  // Create a texture from the surface
  // This uploads the image data to the GPU for efficient rendering.
  m_text_texture
          = SDL_CreateTextureFromSurface(game()->renderer(), text_surface);
  SDL_DestroySurface(
          text_surface
  );  // We can free the surface after creating the texture
  if (m_text_texture == nullptr) { return Log::error_init("m_text_texture"); }
  else
  {
    Log::verbose(Log::indent() + 1,
                 "Created text texture from surface successfully");
  }

  // Successfully created text texture
  return SDL_APP_CONTINUE;
}

ScreenTest::~ScreenTest()
{
  LOG_FRAME_CLASS(ScreenTest);

  // Clean up text texture
  if (m_text_texture != nullptr)
  {
    SDL_DestroyTexture(m_text_texture);
    m_text_texture = nullptr;
    Log::verbose("Destroyed text texture");
  }
  else
  {
    Log::warn("m_text_texture was not initialized, no need to destroy");
  }
}

SDL_AppResult ScreenTest::event(SDL_Event * event)
{
  // React to 'N' key press to switch to main screen
  if (event->type == SDL_EVENT_KEY_DOWN)
  {
    LOG_FRAME_CLASS(ScreenTest);

    // TODO remove outer format call
    Log::verbose("'{}' key pressed ('{}')",
                 SDL_GetKeyName(event->key.key),
                 SDL_GetScancodeName(event->key.scancode));
    if (event->key.key == SDLK_N)
    {
      Log::info("'N' key pressed, switching to main screen...");
      return switch_to_main_screen();
    }
    if (event->key.key == SDLK_ESCAPE)
    {
      Log::info("'ESCAPE' key pressed, quitting game...");
      SDL_Event quit_event;
      quit_event.type = SDL_EVENT_QUIT;
      SDL_PushEvent(
              &quit_event
      );  // Push a quit event to trigger app termination
      return SDL_APP_CONTINUE;
    }
  }
  else if (not SDL_HasKeyboard() && event->type == SDL_EVENT_FINGER_DOWN)
  {
    LOG_FRAME_CLASS(ScreenTest);

    // TODO remove outer call
    Log::verbose("Touch event #{} pressed at ({:.4f}, {:.4f}) with pressure "
                 "{:.4f} and type {}",
                 event->tfinger.fingerID,
                 event->tfinger.x,
                 event->tfinger.y,
                 event->tfinger.pressure,
                 static_cast<int>(event->tfinger.type));
    Log::info("Touch event recieved, switching to main screen...");
    return switch_to_main_screen();
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::update()
{
  LOG_FRAME_CLASS(ScreenTest);

  // Update m_text_texture with new info
  if (auto result = init_text_texture())
  {
    Log::critical("Error occurred while updating text texture, terminating...");
    return result;
  }

  // Set m_rainbow_triangle colors to random values
  // TODO seed rand?
#define RAND_COLOR()                      \
  {static_cast<float>(rand()) / RAND_MAX, \
   static_cast<float>(rand()) / RAND_MAX, \
   static_cast<float>(rand()) / RAND_MAX, \
   1.0f}
  m_rainbow_triangle[0].color = RAND_COLOR();
  m_rainbow_triangle[1].color = RAND_COLOR();
  m_rainbow_triangle[2].color = RAND_COLOR();
#undef RAND_COLOR
  Log::verbose("Updated m_rainbow_triangle vertex colors to random values");


  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::render(SDL_Renderer * renderer)
{
  LOG_FRAME_CLASS(ScreenTest);

  // render m_polygon1
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  if (not SDL_RenderGeometry(renderer, NULL, m_polygon1, VERTEX_COUNT, NULL, 0))
  {
    return Log::critical(
            "Failed to render geometry m_polygon1 with {} vertices: {}",
            6,
            SDL_GetError()
    );
  }
  Log::verbose("Polygon1 rendered");

  // render m_polygon2
  if (not SDL_RenderGeometry(renderer, NULL, m_polygon2, VERTEX_COUNT, NULL, 0))
  {
    return Log::critical(
            "Failed to render geometry m_polygon2 with {} vertices: {}",
            6,
            SDL_GetError()
    );
  }
  Log::verbose("Polygon2 rendered");

  // render m_gradient_rect
  if (not SDL_RenderGeometry(renderer,
                             NULL,
                             m_gradient_rect,
                             GRADIENT_RECT_VERTEX_COUNT,
                             NULL,
                             0))
  {
    return Log::critical(
            "Failed to render geometry m_gradient_rect with {} vertices: {}",
            12,
            SDL_GetError()
    );
  }
  Log::verbose("m_gradient_rect rendered");

  // render m_rainbow_triangle
  if (not SDL_RenderGeometry(renderer, NULL, m_rainbow_triangle, 3, NULL, 0))
  {
    return Log::critical(
            "Failed to render geometry m_rainbow_triangle with {} vertices: {}",
            3,
            SDL_GetError()
    );
  }
  Log::verbose("m_rainbow_triangle rendered");

  // Lastly, render the text texture
  if (m_text_texture != nullptr)
  {
    // position to render text
    float x = 50.0f;
#if __ANDROID__
    // render text lower on Android to accommodate the status bar at the top of
    // the screen
    // TODO better handling of the status bar! extend SDLActivity in Java
    float y = 150.0f;
#else
    float y = 50.0f;
#endif
    SDL_FRect text_rect = {
        x, y, 0.0f, 0.0f
    };  // We only set the x and y position here. The width and
        // height will be determined by the texture
    SDL_GetTextureSize(
            m_text_texture,
            &text_rect.w,
            &text_rect.h
    );  // Get the width and height of the texture
    if (not SDL_RenderTexture(renderer,
                              m_text_texture,
                              NULL,
                              &text_rect))  // Render the texture to the screen
                                            // at the specified position
    {
      return Log::critical("Failed to render text texture: {}", SDL_GetError());
    }
    Log::verbose("Text texture rendered at position ({}, {}) with size ({}, "
                 "{})",
                 text_rect.x,
                 text_rect.y,
                 text_rect.w,
                 text_rect.h);
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::post_render_update() { return SDL_APP_CONTINUE; }

SDL_AppResult ScreenTest::show()
{
  Screen::show();  // Call base class show() to set clear color, etc
  return SDL_APP_CONTINUE;
}

SDL_AppResult ScreenTest::hide() { return SDL_APP_CONTINUE; }

SDL_AppResult ScreenTest::switch_to_main_screen()
{
  // if we have only 1 screen, create a ScreenMain instance and add it to game
  if (game()->screens().size() == 1)
  {
    // if result is not SDL_APP_CONTINUE, return it
    if (SDL_AppResult result = game()->add_screen<ScreenMain>()) return result;
  }

  // Switch to the second screen (ScreenMain)
  if (game()->switch_screen(1))
    return SDL_APP_CONTINUE;
  else
    return Log::error("Failed to switch to main screen: {}", SDL_GetError());
}

}  // namespace bipsy::gaiasim
