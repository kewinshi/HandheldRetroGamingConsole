#ifndef PACMAN_H
#define PACMAN_H

#include "main.h"  // Include main.h for Point
#include "lcd_driver.h"
#include "input.h"
#include <stdint.h>
#include <stdio.h>

// Game constants
#define GRID_WIDTH 17    // 17 tiles wide
#define GRID_HEIGHT 32   // 32 tiles tall
#define TILE_SIZE 10     // 10x10 sprites
#define PACMAN_SPEED 1
#define GHOST_SPEED 1


// Pac-Man sprite (10x10, 0 = black, 1 = yellow)
extern const uint8_t pixel_array_pacman[10][10];

// Ghost sprite (10x10, 0 = black, 1 = red)
extern const uint8_t pixel_array_ghost[10][10];

// Maze with wrapping tunnels (1 = wall, 0 = path/pellet, 2 = tunnel)
extern uint8_t maze[GRID_HEIGHT][GRID_WIDTH];

// Function declaration
void play_pacman(void);

#endif /* PACMAN_H */
