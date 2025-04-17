#include "space_invaders.h"
#include "buzzer_driver.h"

// Custom Space Invader alien pixel array (14x14, 0 = black, 1 = white)
const uint8_t pixel_array_invader[14][14] = {
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}
};

// Custom player ship pixel array (14x14, 0 = black, 1 = cyan)
const uint8_t pixel_array_player[14][14] = {
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

void play_space_invaders(void) {
    LCD_Clear(BLACK);  // Initial clear to set black background
    // Precompute buffers
    uint8_t bg_data[ALIEN_WIDTH * ALIEN_WIDTH * 2];
    uint8_t player_data[PLAYER_WIDTH * PLAYER_WIDTH * 2];
    uint8_t alien_data[ALIEN_WIDTH * ALIEN_WIDTH * 2];
    uint8_t bullet_data[BULLET_WIDTH * BULLET_HEIGHT * 2];

    // Fill buffers
    for (int i = 0; i < ALIEN_WIDTH * ALIEN_WIDTH; i++) {
        bg_data[i * 2] = BLACK >> 8; bg_data[i * 2 + 1] = BLACK & 0xFF;
        player_data[i * 2] = (pixel_array_player[i / 14][i % 14] ? CYAN : BLACK) >> 8;
        player_data[i * 2 + 1] = (pixel_array_player[i / 14][i % 14] ? CYAN : BLACK) & 0xFF;
        alien_data[i * 2] = (pixel_array_invader[i / 14][i % 14] ? WHITE : BLACK) >> 8;
        alien_data[i * 2 + 1] = (pixel_array_invader[i / 14][i % 14] ? WHITE : BLACK) & 0xFF;
    }
    for (int i = 0; i < BULLET_WIDTH * BULLET_HEIGHT; i++) {
        bullet_data[i * 2] = WHITE >> 8; bullet_data[i * 2 + 1] = WHITE & 0xFF;
    }

    Point player = {170 / 2 - 7, LCD_HEIGHT - 20}, prev_player = player;  // Adjusted initial x
    Point aliens[ALIEN_ROWS][ALIEN_COLS], prev_aliens[ALIEN_ROWS][ALIEN_COLS];
    Point bullet = {0, 0, 0}, prev_bullet = bullet;
    int alien_dx = 1, alien_speed = 50, alien_timer = 0;

    // Initialize aliens
    for (int r = 0; r < ALIEN_ROWS; r++) {
        for (int c = 0; c < ALIEN_COLS; c++) {
            aliens[r][c].x = 20 + c * 20;
            aliens[r][c].y = 20 + r * 20;
            aliens[r][c].active = 1;
            prev_aliens[r][c] = aliens[r][c];
        }
    }

    // Initial draw
    LCD_WriteBlockWithOffset(player.x, player.y, PLAYER_WIDTH, PLAYER_WIDTH, player_data);
    for (int r = 0; r < ALIEN_ROWS; r++) {
        for (int c = 0; c < ALIEN_COLS; c++) {
            if (aliens[r][c].active) {
                LCD_WriteBlockWithOffset(aliens[r][c].x, aliens[r][c].y, ALIEN_WIDTH, ALIEN_WIDTH, alien_data);
            }
        }
    }

    while (1) {
        int input = get_input();

        // Player movement with corrected bounds
        if (input == 3 && player.x > 0) player.x -= 5;
        if (input == 4 && player.x < 170 - PLAYER_WIDTH) player.x += 5;  // Adjusted to 170 - 14
        if (input == 1 && !bullet.active) {
            bullet.x = player.x + 7;
            bullet.y = player.y;
            bullet.active = 1;
        }

        // Move bullet
        if (bullet.active) {
        	buzz(10);
            bullet.y -= 10;
            if (bullet.y < 0) bullet.active = 0;
        }

        // Move aliens with corrected bounds
        alien_timer++;
        if (alien_timer >= alien_speed) {
            alien_timer = 0;
            int leftmost_x = 170, rightmost_x = 0;  // Adjusted to 170
            // Find the leftmost and rightmost active aliens
            for (int r = 0; r < ALIEN_ROWS; r++) {
                for (int c = 0; c < ALIEN_COLS; c++) {
                    if (aliens[r][c].active) {
                        if (aliens[r][c].x < leftmost_x) leftmost_x = aliens[r][c].x;
                        if (aliens[r][c].x + ALIEN_WIDTH > rightmost_x) rightmost_x = aliens[r][c].x + ALIEN_WIDTH;
                    }
                }
            }
            // Check bounds and reverse direction if needed
            if (leftmost_x <= 0 && alien_dx < 0) alien_dx = 1;
            if (rightmost_x >= 170 && alien_dx > 0) alien_dx = -1;  // Adjusted to 170
            // Move aliens
            buzz(5);
            for (int r = 0; r < ALIEN_ROWS; r++) {
                for (int c = 0; c < ALIEN_COLS; c++) {
                    if (aliens[r][c].active) {
                        aliens[r][c].x += alien_dx;
                    }
                }
            }
        }

        // Check bullet collision
        for (int r = 0; r < ALIEN_ROWS; r++) {
            for (int c = 0; c < ALIEN_COLS; c++) {
                if (aliens[r][c].active && bullet.active &&
                    bullet.x >= aliens[r][c].x && bullet.x <= aliens[r][c].x + ALIEN_WIDTH &&
                    bullet.y <= aliens[r][c].y + ALIEN_WIDTH) {
                	buzz(50);
                    aliens[r][c].active = 0;
                    bullet.active = 0;
                }
            }
        }

        // Erase previous positions
        LCD_WriteBlockWithOffset(prev_player.x, prev_player.y, PLAYER_WIDTH, PLAYER_WIDTH, bg_data);
        for (int r = 0; r < ALIEN_ROWS; r++) {
            for (int c = 0; c < ALIEN_COLS; c++) {
                if (prev_aliens[r][c].active && (prev_aliens[r][c].x != aliens[r][c].x || prev_aliens[r][c].y != aliens[r][c].y || !aliens[r][c].active)) {
                    LCD_WriteBlockWithOffset(prev_aliens[r][c].x, prev_aliens[r][c].y, ALIEN_WIDTH, ALIEN_WIDTH, bg_data);
                }
            }
        }
        if (prev_bullet.active) {
            LCD_WriteBlockWithOffset(prev_bullet.x, prev_bullet.y, BULLET_WIDTH, BULLET_HEIGHT, bg_data);
        }

        // Draw new positions
        LCD_WriteBlockWithOffset(player.x, player.y, PLAYER_WIDTH, PLAYER_WIDTH, player_data);
        for (int r = 0; r < ALIEN_ROWS; r++) {
            for (int c = 0; c < ALIEN_COLS; c++) {
                if (aliens[r][c].active) {
                    LCD_WriteBlockWithOffset(aliens[r][c].x, aliens[r][c].y, ALIEN_WIDTH, ALIEN_WIDTH, alien_data);
                }
            }
        }
        if (bullet.active) {
            LCD_WriteBlockWithOffset(bullet.x, bullet.y, BULLET_WIDTH, BULLET_HEIGHT, bullet_data);
        }

        // Update previous positions
        prev_player = player;
        for (int r = 0; r < ALIEN_ROWS; r++) {
            for (int c = 0; c < ALIEN_COLS; c++) {
                prev_aliens[r][c] = aliens[r][c];
            }
        }
        prev_bullet = bullet;

        HAL_Delay(25);

        // Win condition
        int active_aliens = 0;
        for (int r = 0; r < ALIEN_ROWS; r++) {
            for (int c = 0; c < ALIEN_COLS; c++) {
                if (aliens[r][c].active) active_aliens++;
            }
        }
        if (active_aliens == 0) {
        	buzz(500);
            LCD_DrawString(50, 100, "You Win!", WHITE, BLACK, 2);
            break;
        }
    }
}
