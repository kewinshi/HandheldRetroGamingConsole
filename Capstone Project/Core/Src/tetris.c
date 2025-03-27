/******************************************************************************
 * Tetris Game - MISRA C Refactored Version with Fast Fall
 *
 * Fast fall added: get_input() returns 5
 * Hard drop shifted: get_input() returns 0
 ******************************************************************************/

#include "tetris.h"
#include "lcd_driver.h"
#include "input.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>  /* For sprintf() */

/* Global Tetris grid: 0 = empty, otherwise stores the color of the locked block */
uint16_t tetris_grid[TETRIS_ROWS][TETRIS_COLS] = {0};

/* Global flag to mark if a hard drop has been used for the current piece */
volatile int hard_drop_used = 0;

/* Global score variable */
static uint32_t score = 0U;
static uint32_t top_score = 0U;

/* Tetris shapes (7 pieces, 4 rotations, each 4x4) */
int shapes[7][4][4][4] =
{
    /* O Tetromino */
    {{{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}},
    /* I Tetromino */
    {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}},
     {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}},
    /* T Tetromino */
    {{{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}},
    /* S Tetromino */
    {{{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}},
    /* Z Tetromino */
    {{{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}},
    /* J Tetromino */
    {{{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}},
    /* L Tetromino */
    {{{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}}
};

/* Colors for each Tetrimino (O, I, T, S, Z, J, L) */
static const uint16_t tetrimino_colors[7] = {
    YELLOW,  /* O */
    CYAN,    /* I */
    PURPLE,  /* T */
    GREEN,   /* S */
    RED,     /* Z */
    BLUE,    /* J */
    ORANGE   /* L */
};

/* Named constants to replace magic numbers */
#define DROP_SPEED       (5U)
#define FAST_FALL_SPEED  (1U)  /* Faster drop rate for fast fall */
#define INVALID_INPUT    (-1)
#define INPUT_ROTATE     (1)
#define INPUT_MOVE_LEFT  (3)
#define INPUT_MOVE_RIGHT (4)
#define INPUT_FAST_FALL  (5)  /* Fast fall input */
#define INPUT_HARD_DROP  (0)  /* Hard drop input */
#define INITIAL_PIECE_Y  (-4)

/* Forward declarations for internal helper functions */
static void DrawSmallBlock(int x, int y, uint16_t color);
static int clear_lines(void);
static void update_score(int lines);
static void redraw_grid(void);
static void redraw_rows(int start_row, int end_row);
static void update_status_display(const Tetrimino *next_piece);
static int is_game_over(void);
static void handle_game_over(void);

void draw_tetris_map(void)
{
    uint32_t row, col;
    LCD_Clear(BLACK);
    /* Draw left border */
    for (row = 0U; row < 20U; row++)
    {
        for (col = 0U; col < 1U; col++)
        {
            DrawWall14x15((int)(col * 14), (int)(row * 14), GREY);
        }
    }
    /* Draw right border */
    for (row = 0U; row < 20U; row++)
    {
        for (col = 0U; col < 1U; col++)
        {
            DrawWall14x15((int)(155 + col * 14), (int)(row * 14), GREY);
        }
    }
    /* Draw bottom border */
    for (row = 0U; row < 3U; row++)
    {
        for (col = 0U; col < 13U; col++)
        {
            DrawWall15x14((int)(1 + col * 14), (int)(280 + row * 15), GREY);
        }
    }
}

/* DrawSmallBlock */
static void DrawSmallBlock(int x, int y, uint16_t color)
{
    clear_area((uint16_t)x, (uint16_t)y, (uint16_t)(x + 7 - 1),
               (uint16_t)(y + 7 - 1), color);
}

/* update_status_display */
static void update_status_display(const Tetrimino *next_piece)
{
    char scoreStr[32];
    sprintf(scoreStr, "Score %lu", (unsigned long)score);
    LCD_DrawString(40, 295, scoreStr, WHITE, GREY, 1);
    LCD_DrawString(100, 295, "Next:", WHITE, GREY, 1);
    for (int m = 0; m < 4; m++)
    {
        for (int n = 0; n < 4; n++)
        {
            int x = 140 + n * 8;
            int y = 285 + m * 8;
            if (shapes[next_piece->shape][next_piece->rotation][m][n] != 0)
            {
                DrawSmallBlock(x, y, next_piece->color);
            }
            else
            {
                clear_area((uint16_t)x, (uint16_t)y, (uint16_t)(x + 7 - 1),
                           (uint16_t)(y + 7 - 1), GREY);
            }
        }
    }
}

/* redraw_rows */
static void redraw_rows(int start_row, int end_row)
{
    int row, col;
    uint16_t x, y;
    for (row = start_row; row <= end_row; row++)
    {
        for (col = 0; col < (int)TETRIS_COLS; col++)
        {
            x = (uint16_t)(col * 14 + 15);
            y = (uint16_t)(row * 14);
            DrawTetrisBlock(x, y, (tetris_grid[row][col] != 0) ? tetris_grid[row][col] : BLACK);
        }
    }
}

/* redraw_grid */
static void redraw_grid(void)
{
    int row, col;
    uint16_t x, y;
    for (row = 0; row < (int)TETRIS_ROWS; row++)
    {
        for (col = 0; col < (int)TETRIS_COLS; col++)
        {
            x = (uint16_t)(col * 14 + 15);
            y = (uint16_t)(row * 14);
            DrawTetrisBlock(x, y, (tetris_grid[row][col] != 0) ? tetris_grid[row][col] : BLACK);
        }
    }
}

/* clear_lines */
static int clear_lines(void)
{
    int linesCleared = 0;
    int row, col;
    int writeRow = (int)TETRIS_ROWS - 1;

    for (row = (int)TETRIS_ROWS - 1; row >= 0; row--)
    {
        bool full = true;
        for (col = 0; col < (int)TETRIS_COLS; col++)
        {
            if (tetris_grid[row][col] == 0)
            {
                full = false;
                break;
            }
        }
        if (full)
        {
            linesCleared++;
        }
        else
        {
            if (writeRow != row)
            {
                for (col = 0; col < (int)TETRIS_COLS; col++)
                {
                    tetris_grid[writeRow][col] = tetris_grid[row][col];
                }
            }
            writeRow--;
        }
    }
    for (row = writeRow; row >= 0; row--)
    {
        for (col = 0; col < (int)TETRIS_COLS; col++)
        {
            tetris_grid[row][col] = 0;
        }
    }
    return linesCleared;
}

/* update_score */
static void update_score(int lines)
{
    if (lines == 1) score += 40U;
    else if (lines == 2) score += 100U;
    else if (lines == 3) score += 300U;
    else if (lines >= 4) score += 1200U;
}

/* is_game_over */
static int is_game_over(void)
{
    for (uint32_t col = 0U; col < TETRIS_COLS; col++)
    {
        if (tetris_grid[0][col] != 0) return 1;
    }
    return 0;
}

/* handle_game_over */
static void handle_game_over(void)
{
    LCD_Clear(BLACK);
    if (score > top_score)
    {
        LCD_DrawString(50, 2 * 20, "High Score!", RED, BLACK, 2);
        top_score = score;
    }
    char scoreStr[20];
    char topScoreStr[20];
    sprintf(scoreStr, "Score: %lu", (unsigned long)score);
    sprintf(topScoreStr, "High Score: %lu", (unsigned long)top_score);
    LCD_DrawString(50, 3 * 20, "Game Over.", WHITE, BLACK, 2.5);
    LCD_DrawString(50, 5 * 20, scoreStr, WHITE, BLACK, 2.5);
    LCD_DrawString(50, 7 * 20, topScoreStr, WHITE, BLACK, 2.5);
    while (get_input() == INVALID_INPUT) HAL_Delay(75);
}

/* play_tetris - Main game loop */
void play_tetris(void)
{
    uint32_t i, j, m, n, idx;
    int curr_input = -1;
    int prev_input = -1;
    int drop_counter = 0;
    int game_over = 0;
    int fast_fall_active = 0;  /* Flag for fast fall */
    int new_positions[4][2];
    int old_positions[4][2];
    Tetrimino current_piece;
    Tetrimino next_piece;

    /* Initialize old_positions */
    for (i = 0U; i < 4U; i++)
    {
        old_positions[i][0] = -1;
        old_positions[i][1] = -1;
    }

    /* Draw static map and clear grid */
    draw_tetris_map();
    for (i = 0U; i < TETRIS_ROWS; i++)
    {
        for (j = 0U; j < TETRIS_COLS; j++)
        {
            tetris_grid[i][j] = 0;
        }
    }

    /* Initialize next_piece */
    next_piece.x = ((int)TETRIS_COLS / 2) - 1;
    next_piece.y = INITIAL_PIECE_Y;
    next_piece.shape = (rand() % 7);
    next_piece.rotation = 0;
    next_piece.color = tetrimino_colors[next_piece.shape];

    /* current_piece becomes next_piece and generate a new next_piece */
    current_piece = next_piece;
    next_piece.x = ((int)TETRIS_COLS / 2) - 1;
    next_piece.y = INITIAL_PIECE_Y;
    next_piece.shape = (rand() % 7);
    next_piece.rotation = 0;
    next_piece.color = tetrimino_colors[next_piece.shape];
    update_status_display(&next_piece);

    /* Main game loop */
    while (game_over == 0)
    {
        prev_input = curr_input;
        curr_input = get_input();

        if (curr_input != INVALID_INPUT)
        {
            if (curr_input == INPUT_MOVE_LEFT)
            {
                move_piece(&current_piece, -1);
            }
            else if (curr_input == INPUT_MOVE_RIGHT)
            {
                move_piece(&current_piece, 1);
            }
            else if (curr_input == INPUT_ROTATE && curr_input != prev_input)
            {
                rotate_piece(&current_piece);
            }
            else if (curr_input == INPUT_HARD_DROP && hard_drop_used == 0)
            {
                hard_drop(&current_piece);
            }
            else if (curr_input == INPUT_FAST_FALL)
            {
                fast_fall_active = 1;  /* Enable fast fall */
            }
        }
        else
        {
            fast_fall_active = 0;  /* Disable fast fall when no input */
        }

        idx = 0U;
        for (m = 0U; m < 4U; m++)
        {
            for (n = 0U; n < 4U; n++)
            {
                if (shapes[current_piece.shape][current_piece.rotation][m][n] != 0)
                {
                    if (idx < 4U)
                    {
                        new_positions[idx][0] = current_piece.x + (int)n;
                        new_positions[idx][1] = current_piece.y + (int)m;
                        idx++;
                    }
                }
            }
        }

        for (i = 0U; i < 4U; i++)
        {
            bool stillOccupied = false;
            if (old_positions[i][0] != -1)
            {
                for (j = 0U; j < 4U; j++)
                {
                    if ((old_positions[i][0] == new_positions[j][0]) &&
                        (old_positions[i][1] == new_positions[j][1]))
                    {
                        stillOccupied = true;
                        break;
                    }
                }
                if (!stillOccupied)
                {
                    int col = old_positions[i][0];
                    int row = old_positions[i][1];
                    if ((row >= 0) && (row < (int)TETRIS_ROWS) &&
                        (col >= 0) && (col < (int)TETRIS_COLS))
                    {
                        DrawTetrisBlock((int)(col * 14 + 15),
                                        (int)(row * 14),
                                        (tetris_grid[row][col] != 0) ? tetris_grid[row][col] : BLACK);
                    }
                }
            }
        }

        for (i = 0U; i < 4U; i++)
        {
            int col = new_positions[i][0];
            int row = new_positions[i][1];
            if ((row >= 0) && (row < (int)TETRIS_ROWS) &&
                (col >= 0) && (col < (int)TETRIS_COLS))
            {
                DrawTetrisBlock((int)(col * 14 + 15),
                                (int)(row * 14),
                                current_piece.color);
            }
        }

        for (i = 0U; i < 4U; i++)
        {
            old_positions[i][0] = new_positions[i][0];
            old_positions[i][1] = new_positions[i][1];
        }

        drop_counter++;
        int effective_drop_speed = fast_fall_active ? FAST_FALL_SPEED : DROP_SPEED;
        if (drop_counter >= effective_drop_speed)
        {
            drop_counter = 0;
            current_piece.y++;
            if (check_collision(&current_piece) != 0)
            {
                current_piece.y--;
                lock_piece(&current_piece);

                int cleared = clear_lines();
                if (cleared > 0)
                {
                    update_score(cleared);
                    redraw_grid();
                }
                else
                {
                    int minRow = current_piece.y;
                    int maxRow = current_piece.y + 3;
                    if (minRow < 0) minRow = 0;
                    if (maxRow >= (int)TETRIS_ROWS) maxRow = (int)TETRIS_ROWS - 1;
                    redraw_rows(minRow, maxRow);
                }

                if (is_game_over() != 0)
                {
                    game_over = 1;
                    break;
                }

                /* Advance: current_piece becomes next_piece, and generate a new next_piece */
                current_piece = next_piece;
                next_piece.x = ((int)TETRIS_COLS / 2) - 1;
                next_piece.y = INITIAL_PIECE_Y;
                next_piece.shape = (rand() % 7);
                next_piece.rotation = 0;
                next_piece.color = tetrimino_colors[next_piece.shape];
                hard_drop_used = 0;
                update_status_display(&next_piece);

                for (i = 0U; i < 4U; i++)
                {
                    old_positions[i][0] = -1;
                    old_positions[i][1] = -1;
                }
                if (check_collision(&current_piece) != 0)
                {
                    game_over = 1;
                }
            }
        }
        HAL_Delay(100);
    }
    handle_game_over();
}

/* check_collision */
int check_collision(Tetrimino *piece)
{
    uint32_t m, n;
    int retVal = 0;
    for (m = 0U; m < 4U; m++)
    {
        for (n = 0U; n < 4U; n++)
        {
            if (shapes[piece->shape][piece->rotation][m][n] != 0)
            {
                int x = piece->x + (int)n;
                int y = piece->y + (int)m;
                if ((x < 0) || (x >= (int)TETRIS_COLS) || (y >= (int)TETRIS_ROWS))
                {
                    retVal = 1;
                    break;
                }
                if ((y >= 0) && (tetris_grid[y][x] != 0))
                {
                    retVal = 1;
                    break;
                }
            }
        }
        if (retVal != 0) break;
    }
    return retVal;
}

/* lock_piece */
void lock_piece(Tetrimino *piece)
{
    uint32_t m, n;
    for (m = 0U; m < 4U; m++)
    {
        for (n = 0U; n < 4U; n++)
        {
            if (shapes[piece->shape][piece->rotation][m][n] != 0)
            {
                int x = piece->x + (int)n;
                int y = piece->y + (int)m;
                if ((x >= 0) && (x < (int)TETRIS_COLS) &&
                    (y >= 0) && (y < (int)TETRIS_ROWS))
                {
                    tetris_grid[y][x] = piece->color;
                }
            }
        }
    }
}

/* move_piece */
void move_piece(Tetrimino *piece, int dx)
{
    piece->x += dx;
    if (check_collision(piece) != 0)
    {
        piece->x -= dx;
    }
}

/* rotate_piece */
void rotate_piece(Tetrimino *piece)
{
    int original_rotation = piece->rotation;
    piece->rotation = (piece->rotation + 1) % 4;
    if (check_collision(piece) != 0)
    {
        piece->rotation = original_rotation;
    }
}

/* hard_drop */
void hard_drop(Tetrimino *piece)
{
    while (check_collision(piece) == 0)
    {
        piece->y++;
    }
    piece->y--; /* Last valid position */
    lock_piece(piece);
    hard_drop_used = 1;
}

/* End of Tetris Game Code */
