#ifndef TETRIS_H
#define TETRIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Global Tetris grid dimensions (adjust if needed) */
#ifndef TETRIS_ROWS
#define TETRIS_ROWS 20U
#endif

#ifndef TETRIS_COLS
#define TETRIS_COLS 10U
#endif

/* Tetrimino structure definition */
typedef struct
{
    int x;        /* Horizontal position in the grid */
    int y;        /* Vertical position in the grid */
    int shape;    /* Index for the shape (0-6) */
    int rotation; /* Rotation state (0-3) */
    uint16_t color; /* Color of the Tetrimino */
} Tetrimino;

extern uint32_t top_score; // Add this line
/* Global variable to track if a hard drop has been used */
extern volatile int hard_drop_used;

/* Function Prototypes */

/**
 * @brief Draws the static parts of the Tetris display (borders, etc.).
 */
void draw_tetris_map(void);

/**
 * @brief Contains the main game loop for the Tetris game.
 */
void play_tetris(void);

/**
 * @brief Checks whether the provided Tetrimino collides with boundaries or locked pieces.
 *
 * @param piece Pointer to the Tetrimino to check.
 * @return int Returns 1 if a collision is detected, 0 otherwise.
 */
int check_collision(Tetrimino *piece);

/**
 * @brief Locks the current Tetrimino into the grid.
 *
 * @param piece Pointer to the Tetrimino to lock.
 */
void lock_piece(Tetrimino *piece);

/**
 * @brief Moves the current Tetrimino horizontally.
 *
 * @param piece Pointer to the Tetrimino.
 * @param dx Horizontal displacement (-1 for left, 1 for right).
 */
void move_piece(Tetrimino *piece, int dx);

/**
 * @brief Rotates the current Tetrimino clockwise.
 *
 * @param piece Pointer to the Tetrimino.
 */
void rotate_piece(Tetrimino *piece);

/**
 * @brief Performs a hard drop on the current Tetrimino (moves it downward instantly).
 *
 * @param piece Pointer to the Tetrimino.
 */
void hard_drop(Tetrimino *piece);

#ifdef __cplusplus
}
#endif

#endif /* TETRIS_H */
