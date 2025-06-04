#include "calc.h"
#include "input.h"
#include "raylib.h"
#include <stdio.h>

void draw_mandelbrot(int width, int height, int maxIter, Color c) {
  ClearBackground(c);
  unsigned char *pixels = calc_mandelbrot(width, height, maxIter);
  Image img = {.data = pixels,
               .width = width,
               .height = height,
               .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
               .mipmaps = 1};
  Texture2D texture = LoadTextureFromImage(img);
  DrawTexture(texture, 0, 0, c);
  UnloadImage(img);
}

int main(void) {
  const int screenWidth = 1600;
  const int screenHeight = 900;

  int xFrom = 0;
  int xTo = 0;
  int yFrom = 0;
  int yTo = 0;

  struct Input *maxIterIn =
      create_input(1, MAX_INPUT_CHARS, 0, screenHeight - 50, "asdf");
  maxIterIn->num = 100;
  struct Input *rIn = create_input(1, 3, 0, screenHeight - 110, "R");
  rIn->num = 0;
  struct Input *gIn = create_input(1, 3, 0, screenHeight - 160, "G");
  gIn->num = 100;
  struct Input *bIn = create_input(1, 3, 0, screenHeight - 210, "B");
  bIn->num = 200;

  InitWindow(screenWidth, screenHeight, "mandelbrot");

  SetTargetFPS(60);

  int framesCounter = 0;

  draw_mandelbrot(screenWidth, screenHeight, maxIterIn->num,
                  (Color){rIn->num, gIn->num, bIn->num, 255});
  while (!WindowShouldClose()) {
    BeginDrawing();

    if ((handle_input(maxIterIn, &framesCounter) && maxIterIn->num > 0) ||
        handle_input(rIn, &framesCounter) ||
        handle_input(gIn, &framesCounter) || handle_input(bIn, &framesCounter))
      draw_mandelbrot(screenWidth, screenHeight, maxIterIn->num,
                      (Color){rIn->num, gIn->num, bIn->num, 255});

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 pos = GetMousePosition();
      printf("%f,%f",pos.x, pos.y);
    }

    EndDrawing();
  }
  destroy_input(maxIterIn);
  destroy_input(rIn);
  destroy_input(gIn);
  destroy_input(bIn);

  CloseWindow();
  return 0;
}
