#define _POSIX_C_SOURCE 200809L
#include "raylib.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_CHARS 6
#define MAX_THREADS 1000
#define MAX_THREAD_LOAD 1000000

struct Input {
  // bitfields just because i can
  unsigned int type : 2;  // 0 str, 1 num, 2 tgl
  unsigned int state : 2; // 0 nor, 1 hov, 2 act
  unsigned int max_chars : 4;
  char *name;
  union {
    char *str;
    int num;
    bool tgl;
  };
};

struct PThreadPool {
  pthread_t *threads;
};

struct MandelbrotSlice {
  int maxIter;
  int height;
  int width;
  int from;
  int to;
  pthread_mutex_t *lock;
  unsigned char *pixels;
};

void *_calc_mandelbrot(void *slice) {
  struct MandelbrotSlice *s = (struct MandelbrotSlice *)slice;
  for (int i = s->from; i < s->to; i++) {
    float x0 = ((float)(i % s->width) / s->width * 2.47) - 2.0;
    float y0 = ((float)(i / s->width) / s->height * 2.24) - 1.12;

    float x1 = 0.0, y1 = 0.0, x2 = 0.0, y2 = 0.0;
    int iter = 0;
    while (x2 + y2 <= 4 && iter < s->maxIter) {
      y1 = 2 * x1 * y1 + y0;
      x1 = x2 - y2 + x0;
      x2 = x1 * x1;
      y2 = y1 * y1;
      iter += 1;
    }
    pthread_mutex_lock(s->lock);
    s->pixels[i] = iter * 256 / s->maxIter;
    pthread_mutex_unlock(s->lock);
  }
  pthread_exit(NULL);
}

unsigned char *calc_mandelbrot(const int width, const int height,
                               const int maxIter) {
  const int size = width * height;
  unsigned char *pixels = malloc(size * sizeof(unsigned char));
  const int load = maxIter * size;
  const int pool_size =  load > MAX_THREADS * MAX_THREAD_LOAD ? MAX_THREADS : load / MAX_THREAD_LOAD;

  // FIXME:
  const int chunk = size / pool_size;

  pthread_t threads[pool_size];
  pthread_mutex_t lock;

  // printf("doing the s:%d ps:%d c:%d\n", size, pool_size, chunk);

  pthread_mutex_init(&lock, NULL);
  for (int i = 0; i < pool_size; i++) {
    struct MandelbrotSlice *slice = malloc(sizeof(struct MandelbrotSlice));
    slice->maxIter = maxIter;
    slice->width = width;
    slice->height = height;
    slice->from = i*chunk;
    // FIXME:
    slice->to = (i+1)*chunk;
    slice->lock = &lock;
    slice->pixels = pixels;
    if (0 != pthread_create(&threads[i], NULL, _calc_mandelbrot, (void *)slice)) 
      perror("failed to create thread");
  }

  for (int i = 0; i < pool_size; i++) 
    if (0 != pthread_join(threads[i], NULL /*(void *)pixels*/))
      perror("failed to join thread");

  pthread_mutex_destroy(&lock);

  return pixels;
}

void handle_input(Rectangle textBox, char name[MAX_INPUT_CHARS + 1],
                  int *framesCounter) {
  int letterCount = strnlen(name, MAX_INPUT_CHARS + 1);
  bool mouseOnText = false;

  if (CheckCollisionPointRec(GetMousePosition(), textBox))
    mouseOnText = true;
  else
    mouseOnText = false;

  if (mouseOnText) {
    SetMouseCursor(MOUSE_CURSOR_IBEAM);

    int key = GetCharPressed();

    // Check if more characters have been pressed on the same frame
    while (key > 0) {
      // NOTE: Only allow numbers
      if ((key >= 48) && (key <= 57) && (letterCount < MAX_INPUT_CHARS)) {
        name[letterCount] = (char)key;
        name[letterCount + 1] = '\0';
        letterCount++;
      }

      key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
      letterCount--;
      if (letterCount < 0)
        letterCount = 0;
      name[letterCount] = '\0';
    }
  } else
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

  if (mouseOnText)
    (*framesCounter)++;
  else
    *framesCounter = 0;

  DrawRectangleRec(textBox, LIGHTGRAY);
  if (mouseOnText)
    DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width,
                       (int)textBox.height, RED);
  else
    DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width,
                       (int)textBox.height, DARKGRAY);
  DrawText(name, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

  if (mouseOnText && letterCount < MAX_INPUT_CHARS &&
      ((*framesCounter / 20) % 2) == 0)
    DrawText("_", (int)textBox.x + 8 + MeasureText(name, 40),
             (int)textBox.y + 12, 40, MAROON);
}

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

  InitWindow(screenWidth, screenHeight, "mandelbrot");

  SetTargetFPS(60);

  Rectangle maxIterBox = {screenWidth - 225, screenHeight - 50, 225, 50};
  char maxIterInput[MAX_INPUT_CHARS + 1] = "\0";
  int framesCounter = 0;

  draw_mandelbrot(screenWidth, screenHeight, maxIter);
  while (!WindowShouldClose()) {
    BeginDrawing();

    handle_input(maxIterBox, maxIterInput, &framesCounter);
    int prevtmp = maxIter;
    maxIter = atoi(maxIterInput);

    if (maxIter > 0 && maxIter != prevtmp)
      draw_mandelbrot(screenWidth, screenHeight, maxIter);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
