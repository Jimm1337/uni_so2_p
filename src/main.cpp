#include <memory>
#include "Game.hpp"

int main() {
  SetTraceLogLevel( LOG_NONE ); // Disable raylib logging

  InitWindow(so::Game::WINDOW_SIZE.x, so::Game::WINDOW_SIZE.y, so::Game::WINDOW_NAME.data());
  SetTargetFPS(so::Game::WINDOW_FPS);

  auto game = std::make_unique<so::Game>();
  game->run();

  CloseWindow();

  return 0;
}
