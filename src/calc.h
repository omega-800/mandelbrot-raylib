struct Section {
  int x;
  int y;
  float zoom;
};

unsigned char *calc_mandelbrot(unsigned char *pixels, const int width,
                               const int height, struct Section *frame,
                               const int maxIter);
