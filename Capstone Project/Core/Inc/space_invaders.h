#ifndef SPACE_INVADERS_H
#define SPACE_INVADERS_H

#include "main.h"
#include "lcd_driver.h"
#include "input.h"

// Game constants
#define PLAYER_WIDTH 14
#define ALIEN_WIDTH 14
#define ALIEN_ROWS 3
#define ALIEN_COLS 5
#define BULLET_WIDTH 2
#define BULLET_HEIGHT 5

// Alien sprite (14x14, 0 = black, 1 = white)
extern const uint8_t pixel_array_invader[14][14];

// Player sprite (14x14, 0 = black, 1 = cyan)
extern const uint8_t pixel_array_player[14][14];

// Function declaration
void play_space_invaders(void);

#endif /* SPACE_INVADERS_H */
