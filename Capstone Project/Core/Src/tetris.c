#include "tetris.h"
#include "lcd_driver.h"
#include "lcd_config.h"
#include "input.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Global Tetris grid: 0 = empty, 1 = locked block
char tetris_grid[TETRIS_ROWS][TETRIS_COLS] = {0};

// Tetris shapes (7 pieces, 4 rotations, each 4x4)
// (These definitions are the same as your original code.)
int shapes[7][4][4][4] = {
    // O Tetromino
    {
        {
            {0,0,0,0},
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        { {0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0} },
        { {0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0} },
        { {0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0} }
    },
    // I Tetromino
    {
        {
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0},
            {0,0,1,0}
        },
        {
            {0,0,0,0},
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,1,0,0}
        }
    },
    // T Tetromino
    {
        {
            {0,0,0,0},
            {0,1,0,0},
            {1,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {1,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },
    // S Tetromino
    {
        {
            {0,0,0,0},
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        }
    },
    // Z Tetromino
    {
        {
            {0,0,0,0},
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },
    // J Tetromino
    {
        {
            {0,0,0,0},
            {1,0,0,0},
            {1,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,1,0},
            {0,0,1,0},
            {0,1,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,1,0,0},
            {0,1,0,0},
            {0,0,0,0}
        }
    },
    // L Tetromino
    {
        {
            {0,0,0,0},
            {0,0,1,0},
            {1,1,1,0},
            {0,0,0,0}
        },
        {
            {0,1,1,0},
            {0,0,1,0},
            {0,0,1,0},
            {0,0,0,0}
        },
        {
            {0,0,0,0},
            {1,1,1,0},
            {1,0,0,0},
            {0,0,0,0}
        },
        {
            {0,1,0,0},
            {0,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        }
    }
};

//---------------------------------------------------------------------
// Helper: draw_tetris_initial_map()
// Draw the static parts of the Tetris display: the border and any locked pieces.
// (Initially, the grid is empty.) This is drawn once at startup.
void draw_tetris_map(void) {
    // Clear the entire screen.
    LCD_Clear(BLACK);

    // --- Draw Left Border (15px wide, 140px tall) using DrawWall5 ---
    // 15px / 5 = 3 blocks wide; 140px / 5 = 28 blocks tall.
    for (int row = 0; row < 56; row++) {
        for (int col = 0; col < 3; col++) {
            // Draw at (x, y) relative to the screen.
            DrawWall5(col * 5, row * 5, GREY);
        }
    }

    // --- Draw Right Border (15px wide, 140px tall) using DrawWall5 ---
    // Right border starts at x = PLAY_X_OFFSET + PLAY_WIDTH = 15 + 280 = 295.
    for (int row = 0; row < 56; row++) {
        for (int col = 0; col < 3; col++) {
            DrawWall5(155 + col * 5, row * 5, GREY);
        }
    }

    // --- Draw Bottom Border (40px tall, 280px wide) using DrawWall (10x10 blocks) ---
    // Bottom border starts at y = PLAY_Y_OFFSET + PLAY_HEIGHT = 0 + 140 = 140.
    // 280px / 10 = 28 blocks wide; 40px / 10 = 4 blocks tall.
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 28; col++) {
            DrawWall(15 + col * 10, 280 + row * 10, GREY);
        }
    }
}



//---------------------------------------------------------------------
// play_tetris()
// This version uses incremental updates (like the snake logic) instead of
// clearing and redrawing the entire display every frame. It only erases (by drawing
// BLACK or the appropriate locked-cell color) those cells that the falling piece no longer occupies,
// then draws the new falling piece blocks.
//---------------------------------------------------------------------
void play_tetris(void) {
    draw_tetris_map();

    // Clear the grid.
    for (int i = 0; i < TETRIS_ROWS; i++) {
        for (int j = 0; j < TETRIS_COLS; j++) {
            tetris_grid[i][j] = 0;
        }
    }

    int game_over = 0;
    int drop_counter = 0;


    // Create the first falling piece.
    Tetrimino current_piece = {TETRIS_COLS / 2 - 1, -4, rand() % 7, 0};

    // old_positions will hold the grid (col, row) of each block of the falling piece from the previous iteration.
    int old_positions[4][2] = { {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1} };

    while (!game_over) {

        // Process user input.
        int curr_input = get_input();

		if (curr_input != -1) {
			if (curr_input == 3) {         // Move left.
				move_piece(&current_piece, -1);
			} else if (curr_input == 4) {  // Move right.
				move_piece(&current_piece, 1);
			} else if (curr_input == 1) {  // Rotate.
				rotate_piece(&current_piece);
			} else if (curr_input == 5 && hard_drop_used == 0) {  // Hard drop.
				hard_drop(&current_piece);
			}
		}
		HAL_Delay(50);




        // Compute the new positions for the current falling piece.
        int new_positions[4][2];
        int idx = 0;
        for (int m = 0; m < 4; m++) {
            for (int n = 0; n < 4; n++) {
                if (shapes[current_piece.shape][current_piece.rotation][m][n]) {
                    new_positions[idx][0] = current_piece.x + n;
                    new_positions[idx][1] = current_piece.y + m;
                    idx++;
                }
            }
        }

        // Erase blocks that are no longer occupied by the falling piece.
        for (int i = 0; i < 4; i++) {
            bool stillOccupied = false;
            if (old_positions[i][0] != -1) {
                for (int j = 0; j < 4; j++) {
                    if (old_positions[i][0] == new_positions[j][0] &&
                        old_positions[i][1] == new_positions[j][1]) {
                        stillOccupied = true;
                        break;
                    }
                }
                if (!stillOccupied) {
                    int col = old_positions[i][0];
                    int row = old_positions[i][1];
                    if (row >= 0 && row < TETRIS_ROWS && col >= 0 && col < TETRIS_COLS) {
                        uint16_t x = col * 14 + 15;
                        uint16_t y = row * 14;
                        // If the cell is locked, redraw the locked block (GREEN); otherwise, clear it (BLACK).
                        if (tetris_grid[row][col])
                            DrawTetrisBlock(x, y, GREEN);
                        else
                            DrawTetrisBlock(x, y, BLACK);
                    }
                }
            }
        }

        // Draw the falling piece in its new positions.
        for (int i = 0; i < 4; i++) {
            int col = new_positions[i][0];
            int row = new_positions[i][1];
            if (row >= 0 && row < TETRIS_ROWS && col >= 0 && col < TETRIS_COLS) {
                uint16_t x = col * 14 + 15;
                uint16_t y = row * 14;
                DrawTetrisBlock(x, y, RED);
            }
        }

        // Update old_positions for the next iteration.
        for (int i = 0; i < 4; i++) {
            old_positions[i][0] = new_positions[i][0];
            old_positions[i][1] = new_positions[i][1];
        }

        // Drop logic: increase drop counter and attempt to move the piece down.
        drop_counter++;
        if (drop_counter >= 5) { // Adjust drop speed as needed.
            drop_counter = 0;
            current_piece.y++;
            if (check_collision(&current_piece)) {
                // Undo the move.
                current_piece.y--;
                // Lock the piece: update the grid and draw locked blocks.
                lock_piece(&current_piece);
                for (int m = 0; m < 4; m++) {
                    for (int n = 0; n < 4; n++) {
                        if (shapes[current_piece.shape][current_piece.rotation][m][n]) {
                            int col = current_piece.x + n;
                            int row = current_piece.y + m;
                            if (row >= 0 && row < TETRIS_ROWS && col >= 0 && col < TETRIS_COLS) {
                                tetris_grid[row][col] = 1;
                                uint16_t x = col * 14 + 15;
                                uint16_t y = row * 14;
                                DrawTetrisBlock(x, y, GREEN);
                            }
                        }
                    }
                }
                // Clear any remnants of the falling piece.
                for (int i = 0; i < 4; i++) {
                    old_positions[i][0] = -1;
                    old_positions[i][1] = -1;
                }
                // Generate a new falling piece.
                current_piece.x = TETRIS_COLS / 2 - 1;
                current_piece.y = -4;
                current_piece.shape = rand() % 7;
                current_piece.rotation = 0;
                hard_drop_used = 0; // Reset hard drop status for the new piece
                if (check_collision(&current_piece)) {
                    game_over = 1;
                }
            }
        }
    } // end game loop

    // Game over: display message.
    LCD_DrawString(20, (TETRIS_ROWS * 10) / 2, "GAME OVER", WHITE, BLACK, 2);
    HAL_Delay(3000);
}

int check_collision(Tetrimino *piece) {
    for (int m = 0; m < 4; m++) {
        for (int n = 0; n < 4; n++) {
            if (shapes[piece->shape][piece->rotation][m][n]) {
                int x = piece->x + n;
                int y = piece->y + m;
                // Check grid boundaries (allow negative y for new pieces).
                if (x < 0 || x >= TETRIS_COLS || y >= TETRIS_ROWS)
                    return 1;
                if (y >= 0 && tetris_grid[y][x])
                    return 1;
            }
        }
    }
    return 0;
}

void lock_piece(Tetrimino *piece) {
    for (int m = 0; m < 4; m++) {
        for (int n = 0; n < 4; n++) {
            if (shapes[piece->shape][piece->rotation][m][n]) {
                int x = piece->x + n;
                int y = piece->y + m;
                if (x >= 0 && x < TETRIS_COLS && y >= 0 && y < TETRIS_ROWS) {
                    tetris_grid[y][x] = 1;
                }
            }
        }
    }
}

void move_piece(Tetrimino *piece, int dx) {
    piece->x += dx;
    if (check_collision(piece)) {
        piece->x -= dx;
    }
}

void rotate_piece(Tetrimino *piece) {
    int original_rotation = piece->rotation;
    piece->rotation = (piece->rotation + 1) % 4;
    if (check_collision(piece)) {
        piece->rotation = original_rotation;
    }
}

// Hard drop: Move piece to bottom instantly.
void hard_drop(Tetrimino *piece) {

    while (!check_collision(piece)) {
        piece->y++;
    }
    piece->y--; // Place at last valid position
    lock_piece(piece);

    // Mark that a hard drop has been used for this piece
    hard_drop_used = 1;
}


