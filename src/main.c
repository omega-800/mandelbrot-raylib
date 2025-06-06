#include "calc.h"
#include "input.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define ZOOM_SCALE 2.0
#define MAX_DBG_CHARS 32

void draw_mandelbrot(int width, int height, struct Section *frame,
                     unsigned char *pixels, Color c) {
  Image img = {.data = pixels,
               .width = width,
               .height = height,
               .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
               .mipmaps = 1};
  Texture2D texture = LoadTextureFromImage(img);
  DrawTexture(texture, 0, 0, c);
  // FIXME:
  // UnloadImage(img);

  char dbg[MAX_DBG_CHARS];
  snprintf((char *)&dbg, MAX_DBG_CHARS, "x: %d, y: %d, Z: %.2f", frame->x,
           frame->y, frame->zoom);
  DrawText(dbg, 0, 0, 20, MAROON);
}

int main(void) {

  struct Section frame = {0, 0, 1.0};

  struct Input *max_iter_in =
      create_input(1, MAX_INPUT_CHARS, 0, SCREEN_HEIGHT - 50, "MAX ITER");
  max_iter_in->num = 100;
  struct Input *r_in = create_input(1, 3, 0, SCREEN_HEIGHT - 210, "R");
  r_in->num = 10;
  struct Input *g_in = create_input(1, 3, 0, SCREEN_HEIGHT - 160, "G");
  g_in->num = 100;
  struct Input *b_in = create_input(1, 3, 0, SCREEN_HEIGHT - 110, "B");
  b_in->num = 255;
  unsigned char *pixels =
      malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(unsigned char));
  calc_mandelbrot(SCREEN_WIDTH, SCREEN_HEIGHT, &frame, pixels,
                  max_iter_in->num);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "mandelbrot");
  SetTraceLogLevel(LOG_WARNING);

  SetTargetFPS(30);

  int frames_counter = 0;

  Color c = (Color){r_in->num, g_in->num, b_in->num, 255};

  draw_mandelbrot(SCREEN_WIDTH, SCREEN_HEIGHT, &frame, pixels, c);
  bool do_draw = false;
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(c);
    if (do_draw)
      calc_mandelbrot(SCREEN_WIDTH, SCREEN_HEIGHT, &frame, pixels,
                      max_iter_in->num);
    draw_mandelbrot(SCREEN_WIDTH, SCREEN_HEIGHT, &frame, pixels, c);
    do_draw = false;

    c = (Color){r_in->num, g_in->num, b_in->num, 255};

    if ((handle_input(max_iter_in, &frames_counter)) ||
        handle_input(r_in, &frames_counter) ||
        handle_input(g_in, &frames_counter) ||
        handle_input(b_in, &frames_counter))
      do_draw = true;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 pos = GetMousePosition();
      float s_x = (SCREEN_WIDTH / ZOOM_SCALE) * 0.5;
      float s_y = (SCREEN_HEIGHT / ZOOM_SCALE) * 0.5;
      int new_x = 0 > pos.x - s_x              ? 0
                  : SCREEN_WIDTH < pos.x + s_x ? SCREEN_WIDTH - (2 * s_x)
                                               : pos.x - s_x;
      int new_y = 0 > pos.y - s_y               ? 0
                  : SCREEN_HEIGHT < pos.y + s_y ? SCREEN_HEIGHT - (2 * s_y)
                                                : pos.y - s_y;
      frame.x += (int)(new_x / frame.zoom);
      frame.y += (int)(new_y / frame.zoom);
      frame.zoom *= ZOOM_SCALE;
      do_draw = true;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      frame.x = 0;
      frame.y = 0;
      frame.zoom = 1.0;
      do_draw = true;
    }

    EndDrawing();
  }
  destroy_input(max_iter_in);
  destroy_input(r_in);
  destroy_input(g_in);
  destroy_input(b_in);
  free(pixels);

  CloseWindow();
  return 0;
}
