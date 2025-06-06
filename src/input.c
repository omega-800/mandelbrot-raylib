#define _POSIX_C_SOURCE 200809L
#include "input.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Input *create_input(unsigned int type, unsigned int max_chars,
                           unsigned int x, unsigned int y, char *name) {
  struct Input *input = malloc(sizeof(struct Input));
  input->type = type;
  input->state = 0;
  input->max_chars = max_chars;
  input->name = name;
  input->box.width = 225;
  input->box.height = 50;
  input->box.x = x;
  input->box.y = y;
  return input;
}

void destroy_input(struct Input *input) {
  if (input->type == 0)
    free(input->str);
  // free(input->name);
  free(input);
}

int digits(int n) {
  int r = 1;
  if (n < 0)
    n = -n;
  while (n > 9) {
    n /= 10;
    r++;
  }
  return r;
}

int handle_input(struct Input *input, int *framesCounter) {
  int letterCount = input->max_chars;
  char *value = malloc((input->max_chars + 1) * sizeof(char));
  switch (input->type) {
  case 0:
    letterCount = strnlen(input->str, input->max_chars + 1);
    strncpy(value, input->str, input->max_chars + 1);
    break;
  case 1:
    letterCount = digits(input->num);
    snprintf(value, input->max_chars + 1, "%d", input->num);
    break;
  case 2:
    letterCount = 1;
    value = "";
    break;
  }
  bool mouseOnText = false;

  if (CheckCollisionPointRec(GetMousePosition(), input->box))
    mouseOnText = true;
  else
    mouseOnText = false;

  if (mouseOnText) {
    SetMouseCursor(MOUSE_CURSOR_IBEAM);

    int key = GetCharPressed();

    // Check if more characters have been pressed on the same frame
    while (key > 0) {
      if (((input->type == 1 && (key >= 48) && (key <= 57)) ||
           (input->type == 0 && (key >= 32) && (key <= 125)))

          && (letterCount < input->max_chars)) {
        value[letterCount] = (char)key;
        value[letterCount + 1] = '\0';
        letterCount++;
      }

      key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
      letterCount--;
      if (letterCount < 0)
        letterCount = 0;
      value[letterCount] = '\0';
    }
  } else
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

  if (mouseOnText)
    (*framesCounter)++;
  else
    *framesCounter = 0;

  DrawRectangleRec(input->box, LIGHTGRAY);
  if (mouseOnText)
    DrawRectangleLines((int)input->box.x, (int)input->box.y,
                       (int)input->box.width, (int)input->box.height, RED);
  else
    DrawRectangleLines((int)input->box.x, (int)input->box.y,
                       (int)input->box.width, (int)input->box.height, DARKGRAY);
  DrawText(value, (int)input->box.x + 5, (int)input->box.y + 8, 40, MAROON);
  DrawText(input->name, (int)input->box.x + input->box.width + 5, (int)input->box.y + 8, 40, MAROON);

  if (mouseOnText && letterCount < MAX_INPUT_CHARS &&
      ((*framesCounter / 20) % 2) == 0)
    DrawText("_", (int)input->box.x + 8 + MeasureText(value, 40),
             (int)input->box.y + 12, 40, MAROON);

  int ret = 0;
  switch (input->type) {
  case 0:
    if (0 == strncmp(input->str, value, input->max_chars + 1))
      break;
    ret = 1;
    strncpy(input->str, value, input->max_chars + 1);
    break;
  case 1: {
    int val = atoi(value);
    if (input->num == val || val == 0)
      break;
    ret = 1;
    input->num = val;
    break;
  }
  case 2:
    break;
  }
  free(value);
  return ret;
}
