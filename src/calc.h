struct Section {
  int x;
  int y;
  float zoom;
};

unsigned char *calc_mandelbrot(const int width, const int height,
                               struct Section *frame, unsigned char *pixels,
                               const int max_iter);
