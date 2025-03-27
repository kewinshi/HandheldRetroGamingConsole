#ifndef SNAKE_H
#define SNAKE_H

#define ROWS 32
#define COLS 17

#include "main.h"  // Include main.h for Point
#include "lcd_driver.h"
#include "input.h"

// A simple structure to hold a coordinate.

// Maximum snake segments (playable area).
#define MAX_SNAKE_LENGTH ((ROWS - 2) * (COLS - 2))

// Global snake body array and current length.
extern Point snake_body[MAX_SNAKE_LENGTH];
extern int snake_length;
extern int top_length;

void play_snake();
void draw_snake_map();
// Returns 1 if snake is alive, 0 if game over.
int move_snake(int xdir, int ydir, int *apple_x, int *apple_y);

#endif /* SNAKE_H */
