#include "screens/ScreenTest.hpp"

#include "Game.hpp" // log_info

namespace bipsy::gaiasim
{

ScreenTest::~ScreenTest()
{
}

SDL_AppResult ScreenTest::init()
{

  // --- Initialize vertex data ---
  // Polygon 1
  log_info("Initializing vertex data for polygon1...", 2);
  // Triangle 1
  polygon1[0].position   = {100, 100};         // top-left vertex
  polygon1[0].color      = {1.0, 0, 0, 1.0};   // red
  polygon1[1].position   = {300, 100};         // top-right vertex
  polygon1[1].color      = {0, 1.0, 0, 1.0};   // green
  polygon1[2].position   = {300, 300};         // bottom-right vertex
  polygon1[2].color      = {0, 0, 1.0, 1.0};   // blue
  // Triangle 2 (shares top-left and bottom-right vertices with triangle 1)
  polygon1[3].position   = {100, 100};         // top-left vertex (same as vertex 0)
  polygon1[3].color      = {1.0, 0, 0, 1.0};   // red (same as vertex 0)
  polygon1[4].position   = {100, 300};         // bottom-left vertex
  polygon1[4].color      = {1.0, 1.0, 0, 1.0}; // yellow 
  polygon1[5].position   = {300, 300};         // bottom-right vertex (same as vertex 2)
  polygon1[5].color      = {0, 0, 1.0, 1.0};   // blue (same as vertex 2)

  // Polygon 2
  log_info("Initializing vertex data for polygon2...", 2);
  // Triangle 1
  polygon2[0].position   = {400, 100};         // top-left vertex
  polygon2[0].color      = {1.0, 0, 0, 1.0};   // red
  polygon2[1].position   = {400, 300};         // bottom-left vertex
  polygon2[1].color      = {1.0, 1.0, 0, 1.0}; // yellow
  polygon2[2].position   = {600, 100};         // top-right vertex
  polygon2[2].color      = {0, 1.0, 0, 1.0};   // green

  // Triangle 2
  polygon2[3].position   = {400, 300};         // bottom-left vertex (same as vertex 1)
  polygon2[3].color      = {1.0, 1.0, 0, 1.0}; // yellow (same as vertex 1)
  polygon2[4].position   = {600, 300};         // bottom-right vertex
  polygon2[4].color      = {0, 0, 1.0, 1.0};   // blue
  polygon2[5].position   = {600, 100};         // top-right vertex (same as vertex 2)
  polygon2[5].color      = {0, 1.0, 0, 1.0};   // green (same as vertex 2


  // Gradient rectangle (4 triangles, 3 vertices each)
  log_info("Initializing vertex data for gradient_rect...", 2);
  // Triangle 1 (left side)
  gradient_rect[0]  = {{100, 100}, {1.0f,0.0f,1.0f,1.0f}, {0.0f,0.0f}}; // top-left vertex (magenta)
  gradient_rect[1]  = {{100, 300}, {1.0f,0.0f,0.0f,1.0f}, {0.0f,1.0f}}; // bottom-left vertex (red)
  gradient_rect[2]  = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)
  // Triangle 2 (right side)
  gradient_rect[3]  = {{300, 100}, {0.0f,1.0f,1.0f,1.0f}, {1.0f,0.0f}}; // top-right vertex (cyan)
  gradient_rect[4]  = {{300, 300}, {0.0f,1.0f,0.0f,1.0f}, {1.0f,1.0f}}; // bottom-right vertex (green)
  gradient_rect[5]  = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)
  // Triangle 3 (top side)
  gradient_rect[6]  = {{100, 100}, {1.0f,0.0f,1.0f,1.0f}, {0.0f,0.0f}}; // top-left vertex (magenta)
  gradient_rect[7]  = {{300, 100}, {0.0f,1.0f,1.0f,1.0f}, {1.0f,0.0f}}; // top-right vertex (cyan)
  gradient_rect[8]  = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)
  // Triangle 4 (bottom side)
  gradient_rect[9]  = {{100, 300}, {1.0f,0.0f,0.0f,1.0f}, {0.0f,1.0f}}; // bottom-left vertex (red)
  gradient_rect[10] = {{300, 300}, {0.0f,1.0f,0.0f,1.0f}, {1.0f,1.0f}}; // bottom-right vertex (green)
  gradient_rect[11] = {{200, 200}, {0.5f,0.5f,0.5f,1.0f}, {0.5f,0.5f}}; // center vertex (gray)

  log_info("Moving gradient_rect down 300 px...", 2);
  // Move gradient_rect down 300 px
  for (int i = 0; i < GRADIENT_RECT_VERTEX_COUNT; i++)
  {
    gradient_rect[i].position.y += 300;
  }

  log_info("Vertex data initialized successfully", 2);

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

SDL_AppResult ScreenTest::render(SDL_Renderer *renderer)
{

  // draw gradient rectangle
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  if (not SDL_RenderGeometry(renderer, NULL,
    polygon1
    // (SDL_Vertex[]){
    //   {{100, 100}, {1,0,0,1}, {0,0}}, // top-left vertex (red)
    //   {{300, 100}, {0,1,0,1}, {1,0}}, // top-right vertex (green)
    //   {{300, 300}, {0,0,1,1}, {1,1}}, // bottom-right vertex (blue)
    //   {{100, 300}, {1,1,0,1}, {0,1}}  // bottom-left vertex (yellow)
    // }
    ,VERTEX_COUNT,
  NULL, 0)
  )
  {
    SDL_Log("num_verticies: VERTEX_COUNT = %d", VERTEX_COUNT);
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
      "Failed to render geometry polygon1: %s", SDL_GetError());
    return SDL_APP_FAILURE; // Return failure result if rendering failed
  }

  if (not SDL_RenderGeometry(renderer, NULL,
    polygon2 ,VERTEX_COUNT, NULL, 0))
  {
    SDL_Log("num_verticies: VERTEX_COUNT = %d", VERTEX_COUNT);
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
      "Failed to render geometry polygon2: %s", SDL_GetError());
    return SDL_APP_FAILURE; // Return failure result if rendering failed
  }

  if (not SDL_RenderGeometry(renderer, NULL,
    gradient_rect ,GRADIENT_RECT_VERTEX_COUNT, NULL, 0))
  {
    SDL_Log("num_verticies: GRADIENT_RECT_VERTEX_COUNT = %d", GRADIENT_RECT_VERTEX_COUNT);
    SDL_LogError(SDL_LOG_CATEGORY_ERROR,
      "Failed to render geometry gradient_rect: %s", SDL_GetError());
    return SDL_APP_FAILURE; // Return failure result if rendering failed
  }

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
