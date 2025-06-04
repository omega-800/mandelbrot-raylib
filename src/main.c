#include "calc.h"
#include "input.h"
#include "raylib.h"
#include <stdlib.h>

void draw_mandelbrot(int width, int height, int maxIter) {
  ClearBackground(BLUE);
  unsigned char *pixels = calc_mandelbrot(width, height, maxIter);
  Image img = {.data = pixels,
               .width = width,
               .height = height,
               .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
               .mipmaps = 1};
  Texture2D texture = LoadTextureFromImage(img);
  DrawTexture(texture, 0, 0, BLUE);
  UnloadImage(img);
}

int main(void) {
  const int screenWidth = 1600;
  const int screenHeight = 900;
  // const int screenWidth = 100;
  // const int screenHeight = 100;
  int maxIter = 100;
  struct Input *maxIterIn = create_input(1, MAX_INPUT_CHARS, screenWidth - 225,
                                         screenHeight - 50, "asdf");
  maxIterIn->num = maxIter;

  InitWindow(screenWidth, screenHeight, "mandelbrot");

  SetTargetFPS(60);

  Rectangle maxIterBox = {screenWidth - 225, screenHeight - 50, 225, 50};
  char maxIterInput[MAX_INPUT_CHARS + 1] = "\0";
  int framesCounter = 0;

  draw_mandelbrot(screenWidth, screenHeight, maxIter);
  while (!WindowShouldClose()) {
    BeginDrawing();

    handle_input(maxIterIn, &framesCounter);
    int prevtmp = maxIter;
    maxIter = atoi(maxIterInput);

    if (maxIter > 0 && maxIter != prevtmp)
      draw_mandelbrot(screenWidth, screenHeight, maxIter);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
