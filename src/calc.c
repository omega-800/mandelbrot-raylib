#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_THREADS 1000
#define MAX_THREAD_LOAD 1000000

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
  free(slice);
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

