#define _POSIX_C_SOURCE 200809L
#include "calc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 16
#define MAX_THREAD_LOAD 100000000

struct MandelbrotSlice {
  int maxIter;
  int height;
  int width;
  int from;
  int to;
  int x;
  int y;
  float zoom;
  pthread_mutex_t *lock;
  unsigned char *pixels;
};

void *_calc_mandelbrot(void *slice) {
  struct MandelbrotSlice *s = (struct MandelbrotSlice *)slice;
  for (int i = s->from; i < s->to; i++) {
    float x0 =
        (((float)(i % s->width / s->zoom) + s->x) / s->width * 2.47) - 2.0;
    float y0 =
        (((float)(i / s->width / s->zoom) + s->y) / s->height * 2.24) - 1.12;

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
  free(slice);
  pthread_exit(NULL);
}

unsigned char *calc_mandelbrot(unsigned char *pixels, const int width,
                               const int height, struct Section *frame,
                               const int maxIter) {
  const int size = width * height;
  const int load = maxIter * size;
  int pool_size = load > MAX_THREADS * MAX_THREAD_LOAD
                            ? MAX_THREADS
                            : load / MAX_THREAD_LOAD;
  if (pool_size == 0) pool_size = 1;
  printf("%d", frame->x);

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
    slice->from = i * chunk;
    // FIXME:
    slice->to = (i + 1) * chunk;
    //FIXME:
    slice->x = frame->x;
    slice->y = frame->y;
    slice->zoom = frame->zoom;
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
