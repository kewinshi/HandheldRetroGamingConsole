#include "tetris.h"
#include <stdlib.h>
#include <time.h>

// Tetris grid
char tetris_grid[TETRIS_ROWS][TETRIS_COLS] = {0};

// Tetris shapes (Define all shapes and rotations)
int shapes[7][4][4][4] = {
    // O Tetromino
    {
        // Rotations 0-3 (same for O Tetromino)
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        }
    },
    // I Tetromino
    {
        // Rotation 0
        {
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0}
        },
        // Rotation 2
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 1},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0}
        }
    },
    // T Tetromino
    {
        // Rotation 0
        {
            {0, 0, 0, 0},
            {0, 1, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 1, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    },
    // S Tetromino
    {
        // Rotation 0
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        }
    },
    // Z Tetromino
    {
        // Rotation 0
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {0, 0, 0, 0},
            {1, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    },
    // J Tetromino
    {
        // Rotation 0
        {
            {0, 0, 0, 0},
            {1, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 1, 1, 0},
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 0, 0}
        }
    },
    // L Tetromino
    {
        // Rotation 0
        {
            {0, 0, 0, 0},
            {0, 0, 1, 0},
            {1, 1, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 1
        {
            {0, 1, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 0}
        },
        // Rotation 2
        {
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {1, 0, 0, 0},
            {0, 0, 0, 0}
        },
        // Rotation 3
        {
            {0, 1, 0, 0},
            {0, 1, 0, 0},
            {0, 1, 1, 0},
            {0, 0, 0, 0}
        }
    }
};




void play_tetris() {
	srand(0);
    uart_print("\x1b[H"); // Reset cursor
    uart_print("\e[2J");  // Clear screen

    int game_over = 0;
    int curr_input = -1, prev_input = -1, game_select = 1;
    int piece_select = rand() % 7;
    Tetrimino current_piece = {TETRIS_COLS / 2 - 1, -4, piece_select, 0};

    while (game_select == 1) {
        HAL_Delay(60); // Adjust delay as needed

        // Get user input
        get_input(&curr_input, &prev_input, &game_select);

        // Handle user input
        if (curr_input != -1 && curr_input != prev_input) {
            if (curr_input == 1) { // Move left
                move_piece(&current_piece, -1);
            } else if (curr_input == 3) { // Move right
                move_piece(&current_piece, 1);
            } else if (curr_input == 0) { // Rotate
                rotate_piece(&current_piece);
            } else if (curr_input == 2) { // Hard drop
                hard_drop(&current_piece);
            }
        }

        // Move down every few iterations or based on a timer
        static int drop_counter = 0;
        drop_counter++;
        if (drop_counter >= 5) { // Adjust drop speed as needed
            drop_counter = 0;
            // Move down
            current_piece.y++;
            if (check_collision(&current_piece)) {
                current_piece.y--; // Undo movement
                lock_piece(&current_piece);
                clear_lines();

                // Generate a new piece
                piece_select = rand() % 7;
                uart_int_print(piece_select);
                uart_print("test");
                current_piece = (Tetrimino){TETRIS_COLS / 2 - 1, -4, piece_select, 0};
                if (check_collision(&current_piece)) {
                    game_over = 1;
                    uart_print("Game Over\r\n");
                }
            }
        }

        // Redraw the grid with the current piece
        draw_tetris_grid(&current_piece);
    }
}

void draw_tetris_grid(Tetrimino *current_piece) {
    uart_print("\x1b[H"); // Reset cursor to top-left
    uart_print("┌");
    for (int i = 0; i < TETRIS_COLS; i++) uart_print("─");
    uart_print("┐\r\n");

    for (int i = 0; i < TETRIS_ROWS; i++) {
        uart_print("│");
        for (int j = 0; j < TETRIS_COLS; j++) {
            int cell_filled = tetris_grid[i][j];

            // Check if current cell is part of the moving piece
            int is_current_piece = 0;
            for (int m = 0; m < 4; m++) {
                for (int n = 0; n < 4; n++) {
                    if (shapes[current_piece->shape][current_piece->rotation][m][n]) {
                        int x = current_piece->x + n;
                        int y = current_piece->y + m;
                        if (x == j && y == i) {
                            is_current_piece = 1;
                            break;
                        }
                    }
                }
                if (is_current_piece) break;
            }

            if (cell_filled || is_current_piece)
                uart_print("o");
            else
                uart_print(" ");
        }
        uart_print("│\r\n");
    }

    uart_print("└");
    for (int i = 0; i < TETRIS_COLS; i++) uart_print("─");
    uart_print("┘\r\n");
}

int check_collision(Tetrimino *piece) {
    for (int m = 0; m < 4; m++) {
        for (int n = 0; n < 4; n++) {
            if (shapes[piece->shape][piece->rotation][m][n]) {
                int x = piece->x + n;
                int y = piece->y + m;

                // Check boundaries
                if (x < 0 || x >= TETRIS_COLS || y >= TETRIS_ROWS) {
                    return 1; // Collision with walls or floor
                }
                // Only check collision with grid if y >= 0
                if (y >= 0 && tetris_grid[y][x]) {
                    return 1; // Collision with locked pieces
                }
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

void clear_lines() {
    for (int i = TETRIS_ROWS - 1; i >= 0; i--) {
        int full_line = 1;
        for (int j = 0; j < TETRIS_COLS; j++) {
            if (!tetris_grid[i][j]) {
                full_line = 0;
                break;
            }
        }
        if (full_line) {
            // Move all rows above down
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < TETRIS_COLS; j++) {
                    tetris_grid[k][j] = tetris_grid[k - 1][j];
                }
            }
            // Clear top row
            for (int j = 0; j < TETRIS_COLS; j++) {
                tetris_grid[0][j] = 0;
            }
            i++; // Re-check the same row after shifting
        }
    }
}

void rotate_piece(Tetrimino *piece) {
    int original_rotation = piece->rotation;
    piece->rotation = (piece->rotation + 1) % 4;
    if (check_collision(piece)) {
        piece->rotation = original_rotation; // Undo rotation if collision occurs
    }
}

void move_piece(Tetrimino *piece, int dx) {
    piece->x += dx;
    if (check_collision(piece)) {
        piece->x -= dx; // Undo movement if collision occurs
    }
}

void hard_drop(Tetrimino *piece) {
    while (!check_collision(piece)) {
        piece->y++;
    }
    piece->y--; // Step back to last valid position
    lock_piece(piece);
    clear_lines();

    // Generate a new piece
    piece->x = TETRIS_COLS / 2 - 1;
    piece->y = -4;
    piece->shape = rand() % 7;
    piece->rotation = 0;
    if (check_collision(piece)) {
        uart_print("Game Over\r\n");
        // Implement game over handling (e.g., set a flag or exit)
    }
}

void print_shape(int shape_index, int rotation_index) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shapes[shape_index][rotation_index][i][j])
                uart_print("o");
            else
                uart_print(".");
        }
        uart_print("\r\n");
    }
    HAL_Delay(1000);
}
