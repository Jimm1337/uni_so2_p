#include <raylib.h>
#include "Object.hpp"
#include "Sprite.hpp"

int main() {
  InitWindow(800, 600, "Hello, World!");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Hello, World!", 10, 10, 20, DARKGRAY);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
