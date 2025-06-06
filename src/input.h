#include "raylib.h"

#define MAX_INPUT_CHARS 6

struct Input {
  Rectangle box;
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

int handle_input(struct Input *input, int *frames_counter);
struct Input *create_input(unsigned int type, unsigned int max_chars,
                           unsigned int x, unsigned int y, char *name);
void destroy_input(struct Input *input);

