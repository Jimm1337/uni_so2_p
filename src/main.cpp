#include <fmt/core.h>
#include "Game.hpp"

int main() {
  fmt::print("{}\n", RES_PATH);

//  InitWindow(800, 600, "Hello, World!");
//
//  SetTargetFPS(60);
//
//  while (!WindowShouldClose()) {
//    BeginDrawing();
//    ClearBackground(RAYWHITE);
//    DrawPixel(10, 10, RED);
//    EndDrawing();
//  }
//
//  CloseWindow();

  InitWindow(so::Game::WINDOW_SIZE.x, so::Game::WINDOW_SIZE.y, so::Game::WINDOW_NAME.data());
  SetTargetFPS(so::Game::WINDOW_FPS);

  so::Game game;
  game.run();

  CloseWindow();

  return 0;
}
