// snake.h
#ifndef SNAKE_H
#define SNAKE_H

#define ROWS 32
#define COLS 17

int map[ROWS - 2][COLS - 2] = {0};

static int snake_x;
static int snake_y;

void play_snake();

void draw_snake_map();  // Declaration for snake map function

void draw_apple(int *apple_x, int *apple_y);

void move_snake();
#endif /* SNAKE_H */
