#ifndef TETRIS_H
#define TETRIS_H

#include "main.h"

// Tetris grid dimensions
#define TETRIS_ROWS 20
#define TETRIS_COLS 10

// Tetris shapes
extern int shapes[7][4][4][4];  // Shapes array with rotations

// Struct to represent a Tetrimino
typedef struct {
    int x, y;         // Position of the Tetrimino
    int shape;        // Shape index (0-6 for different pieces)
    int rotation;     // Rotation state (0-3)
} Tetrimino;

// Global Tetris grid
extern char tetris_grid[TETRIS_ROWS][TETRIS_COLS];

// Function declarations
void play_tetris();
void draw_tetris_grid();
void place_tetrimino(Tetrimino *piece);
int check_collision(Tetrimino *piece);
void lock_piece(Tetrimino *piece);
void clear_lines();
void print_shape(int shape_index, int rotate_index);
void move_piece(Tetrimino *piece, int dx);
void rotate_piece(Tetrimino *piece);
void hard_drop(Tetrimino *piece);

#endif // TETRIS_H
