#include "pacman.h"

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

// Ghost sprite (10x10, 0 = black, 1 = red)
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

// Maze with wrapping tunnels (1 = wall, 0 = path/pellet, 2 = tunnel)
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
    {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1},
    {1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1},
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
    int iteration_count = 0; // Counter for iterations
    const int MODE_SWITCH_INTERVAL = 50; // Switch every 50 iterations
    Point scatter_target = {15, 1}; // Scatter target (top-right corner)

    uint8_t bg_data[TILE_SIZE * TILE_SIZE * 2];
    uint8_t pacman_data[TILE_SIZE * TILE_SIZE * 2];
    uint8_t ghost_data[TILE_SIZE * TILE_SIZE * 2];
    uint8_t wall_data[TILE_SIZE * TILE_SIZE * 2];
    uint8_t pellet_data[4 * 4 * 2];  // 4x4 pellet

    for (int i = 0; i < TILE_SIZE * TILE_SIZE; i++) {
        bg_data[i * 2] = BLACK >> 8; bg_data[i * 2 + 1] = BLACK & 0xFF;
        pacman_data[i * 2] = (pixel_array_pacman[i / 10][i % 10] ? YELLOW : BLACK) >> 8;
        pacman_data[i * 2 + 1] = (pixel_array_pacman[i / 10][i % 10] ? YELLOW : BLACK) & 0xFF;
        ghost_data[i * 2] = (pixel_array_ghost[i / 10][i % 10] ? RED : BLACK) >> 8;
        ghost_data[i * 2 + 1] = (pixel_array_ghost[i / 10][i % 10] ? RED : BLACK) & 0xFF;
        wall_data[i * 2] = DARK_GREY >> 8; wall_data[i * 2 + 1] = DARK_GREY & 0xFF;
    }
    for (int i = 0; i < 4 * 4; i++) {
        pellet_data[i * 2] = WHITE >> 8; pellet_data[i * 2 + 1] = WHITE & 0xFF;
    }

    Point pacman = {1, 1}, prev_pacman = pacman;
    Point ghost = {8, 17}, prev_ghost = ghost; // Center of ghost box
    int pacman_dx = 0, pacman_dy = 0;
    int ghost_dx = -1, ghost_dy = 0;
    int score = 0;

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
    LCD_WriteBlockWithOffset(ghost.x * TILE_SIZE, ghost.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, ghost_data);

    while (1) {
        int input = get_input();
        if (input == 2) { pacman_dx = 0; pacman_dy = -PACMAN_SPEED; }
        if (input == 5) { pacman_dx = 0; pacman_dy = PACMAN_SPEED; }
        if (input == 3) { pacman_dx = -PACMAN_SPEED; pacman_dy = 0; }
        if (input == 4) { pacman_dx = PACMAN_SPEED; pacman_dy = 0; }

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

        // Mode switching every 50 iterations
        iteration_count++;
        if (iteration_count >= MODE_SWITCH_INTERVAL) {
            mode = !mode; // Toggle between 0 (chase) and 1 (scatter)
            iteration_count = 0; // Reset counter
        }

        // Ghost movement logic
        if (mode == 0) { // Chase mode
            if (pacman.x < ghost.x && maze[ghost.y][ghost.x - 1] != 1) ghost_dx = -GHOST_SPEED;
            else if (pacman.x > ghost.x && maze[ghost.y][ghost.x + 1] != 1) ghost_dx = GHOST_SPEED;
            else ghost_dx = 0;
            if (pacman.y < ghost.y && maze[ghost.y - 1][ghost.x] != 1) ghost_dy = -GHOST_SPEED;
            else if (pacman.y > ghost.y && maze[ghost.y + 1][ghost.x] != 1) ghost_dy = GHOST_SPEED;
            else ghost_dy = 0;
        } else { // Scatter mode
            if (scatter_target.x < ghost.x && maze[ghost.y][ghost.x - 1] != 1) ghost_dx = -GHOST_SPEED;
            else if (scatter_target.x > ghost.x && maze[ghost.y][ghost.x + 1] != 1) ghost_dx = GHOST_SPEED;
            else ghost_dx = 0;
            if (scatter_target.y < ghost.y && maze[ghost.y - 1][ghost.x] != 1) ghost_dy = -GHOST_SPEED;
            else if (scatter_target.y > ghost.y && maze[ghost.y + 1][ghost.x] != 1) ghost_dy = GHOST_SPEED;
            else ghost_dy = 0;
        }

        next_x = ghost.x + ghost_dx;
        next_y = ghost.y + ghost_dy;

        // Wrapping logic for Ghost
        if (next_x < 0 && maze[next_y][GRID_WIDTH - 1] != 1) next_x = GRID_WIDTH - 1;
        else if (next_x >= GRID_WIDTH && maze[next_y][0] != 1) next_x = 0;
        if (next_x >= 0 && next_x < GRID_WIDTH && next_y >= 0 && next_y < GRID_HEIGHT && maze[next_y][next_x] != 1) {
            ghost.x = next_x;
            ghost.y = next_y;
        }

        LCD_WriteBlockWithOffset(prev_pacman.x * TILE_SIZE, prev_pacman.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, bg_data);
        LCD_WriteBlockWithOffset(prev_ghost.x * TILE_SIZE, prev_ghost.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, bg_data);
        LCD_WriteBlockWithOffset(pacman.x * TILE_SIZE, pacman.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, pacman_data);
        LCD_WriteBlockWithOffset(ghost.x * TILE_SIZE, ghost.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, ghost_data);

        prev_pacman = pacman;
        prev_ghost = ghost;

        if (pacman.x == ghost.x && pacman.y == ghost.y) {
            LCD_DrawString(50, 100, "Game Over", WHITE, BLACK, 2);
            char score_str[20];
            sprintf(score_str, "Score: %d", score);
            LCD_DrawString(50, 120, score_str, WHITE, BLACK, 2);
            break;
        }

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
            break;
        }

        HAL_Delay(200);
    }
}
