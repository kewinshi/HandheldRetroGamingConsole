#include "pacman.h"
#include <stdbool.h>

#define NUM_GHOSTS 3

// Pac-Man sprite (10x10, 0 = black, 1 = yellow)
const uint8_t pixel_array_pacman[10][10] = {
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}
};

// Ghost sprite (10x10, 0 = black, 1 = color)
const uint8_t pixel_array_ghost[10][10] = {
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 1}
};

// Maze with blue walls, wrapping tunnels, and NUM_GHOSTS x 1 ghost box (1 = wall, 0 = path/pellet, 2 = tunnel)
uint8_t maze[GRID_HEIGHT][GRID_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1}, // Row 13
    {1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1}, // Row 14: gate at columns 8, 9, 10
    {1,0,1,1,1,1,1,1,0,0,0,1,1,1,1,0,1}, // Row 15: ghost box (cols 8-10)
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1}, // Row 16
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {1,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void play_pacman(void) {
    LCD_Clear(BLACK);

    int mode = 1; // Start in scatter mode (0 = chase, 1 = scatter)
    int iteration_count = 0;
    const int MODE_SWITCH_INTERVAL = 50;
    Point scatter_targets[NUM_GHOSTS] = {{15, 1}, {1, 1}, {15, 30}}; // Top-right, top-left, bottom-right

    uint8_t bg_data[TILE_SIZE * TILE_SIZE * 2];
    uint8_t pacman_data[TILE_SIZE * TILE_SIZE * 2];
    uint8_t ghost_data[NUM_GHOSTS][TILE_SIZE * TILE_SIZE * 2]; // Array for each ghost's color
    uint8_t wall_data[TILE_SIZE * TILE_SIZE * 2];
    uint8_t pellet_data[4 * 4 * 2]; // 4x4 pellet

    // Define ghost colors
    uint16_t ghost_colors[NUM_GHOSTS] = {RED, 0xF81F, 0x07FF}; // Red, Pink, Cyan (RGB565)

    // Initialize sprite data
    for (int i = 0; i < TILE_SIZE * TILE_SIZE; i++) {
        bg_data[i * 2] = BLACK >> 8;
        bg_data[i * 2 + 1] = BLACK & 0xFF;
        pacman_data[i * 2] = (pixel_array_pacman[i / 10][i % 10] ? YELLOW : BLACK) >> 8;
        pacman_data[i * 2 + 1] = (pixel_array_pacman[i / 10][i % 10] ? YELLOW : BLACK) & 0xFF;
        wall_data[i * 2] = 0x001F >> 8; // Blue (RGB565: 0x001F)
        wall_data[i * 2 + 1] = 0x001F & 0xFF;
        // Set each ghost's color
        for (int g = 0; g < NUM_GHOSTS; g++) {
            ghost_data[g][i * 2] = (pixel_array_ghost[i / 10][i % 10] ? ghost_colors[g] : BLACK) >> 8;
            ghost_data[g][i * 2 + 1] = (pixel_array_ghost[i / 10][i % 10] ? ghost_colors[g] : BLACK) & 0xFF;
        }
    }
    for (int i = 0; i < 4 * 4; i++) {
        pellet_data[i * 2] = WHITE >> 8;
        pellet_data[i * 2 + 1] = WHITE & 0xFF;
    }

    Point pacman = {1, 1}, prev_pacman = pacman;
    Point ghosts[NUM_GHOSTS];
    Point prev_ghosts[NUM_GHOSTS];
    int pacman_dx = 0, pacman_dy = 0;
    int ghost_dx[NUM_GHOSTS];
    int ghost_dy[NUM_GHOSTS];
    int ghost_release_timers[NUM_GHOSTS] = {0, 50, 100}; // Release at 0, 50, 100 iterations
    bool ghost_active[NUM_GHOSTS] = {false, false, false};
    int score = 0;

    // Initialize ghosts inside the horizontal box
    for (int i = 0; i < NUM_GHOSTS; i++) {
        ghosts[i] = (Point){8 + i, 15}; // Columns 8, 9, 10, row 15
        prev_ghosts[i] = ghosts[i];
        ghost_dx[i] = 0;
        ghost_dy[i] = -GHOST_SPEED; // Move up to exit
    }

    // Draw initial maze
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (maze[y][x] == 1) {
                LCD_WriteBlockWithOffset(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, wall_data);
            } else if (maze[y][x] != 2) { // Don't draw pellets in tunnels
                LCD_WriteBlockWithOffset(x * TILE_SIZE + 3, y * TILE_SIZE + 3, 4, 4, pellet_data);
            }
        }
    }
    LCD_WriteBlockWithOffset(pacman.x * TILE_SIZE, pacman.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, pacman_data);
    for (int i = 0; i < NUM_GHOSTS; i++) {
        LCD_WriteBlockWithOffset(ghosts[i].x * TILE_SIZE, ghosts[i].y * TILE_SIZE, TILE_SIZE, TILE_SIZE, ghost_data[i]);
    }

    while (1) {
        int input = get_input();
        if (input == 2) { pacman_dx = 0; pacman_dy = -PACMAN_SPEED; } // Up
        if (input == 5) { pacman_dx = 0; pacman_dy = PACMAN_SPEED; }  // Down
        if (input == 3) { pacman_dx = -PACMAN_SPEED; pacman_dy = 0; } // Left
        if (input == 4) { pacman_dx = PACMAN_SPEED; pacman_dy = 0; }  // Right

        int next_x = pacman.x + pacman_dx;
        int next_y = pacman.y + pacman_dy;

        // Wrapping logic for Pac-Man
        if (next_x < 0 && maze[next_y][GRID_WIDTH - 1] != 1) next_x = GRID_WIDTH - 1;
        else if (next_x >= GRID_WIDTH && maze[next_y][0] != 1) next_x = 0;
        if (next_x >= 0 && next_x < GRID_WIDTH && next_y >= 0 && next_y < GRID_HEIGHT && maze[next_y][next_x] != 1) {
            pacman.x = next_x;
            pacman.y = next_y;
            if (maze[pacman.y][pacman.x] == 0) {
                maze[pacman.y][pacman.x] = 3; // Mark as eaten
                score += 10;
            }
        }

        // Mode switching
        iteration_count++;
        if (iteration_count >= MODE_SWITCH_INTERVAL) {
            mode = !mode; // Toggle between chase and scatter
            iteration_count = 0;
        }

        // Ghost movement
        for (int i = 0; i < NUM_GHOSTS; i++) {
            if (!ghost_active[i]) {
                ghost_release_timers[i]--;
                if (ghost_release_timers[i] <= 0) {
                    ghost_active[i] = true; // Ghost is released
                }
                continue; // Stay in place until released
            }

            // Chase or scatter target
            Point target = (mode == 0) ? pacman : scatter_targets[i];
            if (target.x < ghosts[i].x && maze[ghosts[i].y][ghosts[i].x - 1] != 1)
                ghost_dx[i] = -GHOST_SPEED;
            else if (target.x > ghosts[i].x && maze[ghosts[i].y][ghosts[i].x + 1] != 1)
                ghost_dx[i] = GHOST_SPEED;
            else
                ghost_dx[i] = 0;
            if (target.y < ghosts[i].y && maze[ghosts[i].y - 1][ghosts[i].x] != 1)
                ghost_dy[i] = -GHOST_SPEED;
            else if (target.y > ghosts[i].y && maze[ghosts[i].y + 1][ghosts[i].x] != 1)
                ghost_dy[i] = GHOST_SPEED;
            else
                ghost_dy[i] = 0;

            next_x = ghosts[i].x + ghost_dx[i];
            next_y = ghosts[i].y + ghost_dy[i];

            // Wrapping logic for ghosts
            if (next_x < 0 && maze[next_y][GRID_WIDTH - 1] != 1) next_x = GRID_WIDTH - 1;
            else if (next_x >= GRID_WIDTH && maze[next_y][0] != 1) next_x = 0;
            if (next_x >= 0 && next_x < GRID_WIDTH && next_y >= 0 && next_y < GRID_HEIGHT && maze[next_y][next_x] != 1) {
                ghosts[i].x = next_x;
                ghosts[i].y = next_y;
            }
        }

        // Update display
        LCD_WriteBlockWithOffset(prev_pacman.x * TILE_SIZE, prev_pacman.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, bg_data);
        for (int i = 0; i < NUM_GHOSTS; i++) {
            LCD_WriteBlockWithOffset(prev_ghosts[i].x * TILE_SIZE, prev_ghosts[i].y * TILE_SIZE, TILE_SIZE, TILE_SIZE, bg_data);
        }
        LCD_WriteBlockWithOffset(pacman.x * TILE_SIZE, pacman.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, pacman_data);
        for (int i = 0; i < NUM_GHOSTS; i++) {
            LCD_WriteBlockWithOffset(ghosts[i].x * TILE_SIZE, ghosts[i].y * TILE_SIZE, TILE_SIZE, TILE_SIZE, ghost_data[i]);
        }
        for (int i = 0; i < NUM_GHOSTS; i++) {
            prev_ghosts[i] = ghosts[i];
        }
        prev_pacman = pacman;

        // Check for collision with any ghost
        for (int i = 0; i < NUM_GHOSTS; i++) {
            if (ghost_active[i] && pacman.x == ghosts[i].x && pacman.y == ghosts[i].y) {
                LCD_DrawString(50, 100, "Game Over", WHITE, BLACK, 2);
                char score_str[20];
                sprintf(score_str, "Score: %d", score);
                LCD_DrawString(50, 120, score_str, WHITE, BLACK, 2);
                return;
            }
        }

        // Check for win condition
        int pellets_left = 0;
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (maze[y][x] == 0) pellets_left++;
            }
        }
        if (pellets_left == 0) {
            LCD_DrawString(50, 100, "You Win!", WHITE, BLACK, 2);
            char score_str[20];
            sprintf(score_str, "Score: %d", score);
            LCD_DrawString(50, 120, score_str, WHITE, BLACK, 2);
            return;
        }

        HAL_Delay(200);
    }
}
