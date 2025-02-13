#ifndef PACMAN_H
#define PACMAN_H

#include <stdint.h>

// Define colors (RGB565 for example)
#define BG_COLOR        0x0000
#define PACMAN_COLOR    0xFFE0
#define WALL_COLOR      0x001F
#define DOT_COLOR       0xFFFF

// Each cell is 8×8 pixels
#define PACMAN_CELL_SIZE  8

void play_pacman(int ROWS, int COLS);

void move_pacman(int ROWS, int COLS,
                 int x[], int y[],
                 int *head, int *tail,
                 int *xdir, int *ydir,
                 char map_pacman[ROWS][COLS]);

void draw_pacman_map(int ROWS, int COLS,
                     char map_pacman[ROWS][COLS]);

#endif // PACMAN_H
