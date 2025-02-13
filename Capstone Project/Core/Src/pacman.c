//#include "pacman.h"
//#include "input.h"
//#include "lcd_driver.h"
//#include <stdlib.h>
//#include <stdio.h>
//
///**
// * @brief Main Pac-Man function
// */
//void play_pacman(int ROWS, int COLS)
//{
//    // Clear screen with background color
//    LCD_Clear(BG_COLOR);
//
//    int x[1000], y[1000];
//    int head = 0, tail = 0;
//    x[head] = COLS / 2;
//    y[head] = 14;
//
//    int xdir = 0, ydir = 0;
//
//    // 2D map array
//    char map_pacman[ROWS][COLS];
//
//    // Build the map and draw it
//    draw_pacman_map(ROWS, COLS, map_pacman);
//
//    int curr_input = -1, prev_input = -1, game_select = 1;
//
//    // Game loop
//    while (game_select == 1)
//    {
//        // Poll input a few times for smoother movement
//        for (int i = 0; i < 5; i++)
//        {
//            get_input(&curr_input, &prev_input, &game_select);
//
//            if (curr_input == 0) {        // W
//                xdir = 0;  ydir = -1;
//            } else if (curr_input == 1) { // A
//                xdir = -1; ydir = 0;
//            } else if (curr_input == 2) { // S
//                xdir = 0;  ydir = 1;
//            } else if (curr_input == 3) { // D
//                xdir = 1;  ydir = 0;
//            }
//
//            HAL_Delay(50);
//        }
//
//        move_pacman(ROWS, COLS, x, y, &head, &tail, &xdir, &ydir, map_pacman);
//    }
//}
//
///**
// * @brief Move Pac-Man if no wall is in the way
// */
//void move_pacman(int ROWS, int COLS,
//                 int x[], int y[],
//                 int *head, int *tail,
//                 int *xdir, int *ydir,
//                 char map_pacman[ROWS][COLS])
//{
//    int next_x = (x[*head] + *xdir + COLS) % COLS;
//    int next_y = (y[*head] + *ydir + ROWS) % ROWS;
//
//    // Check if next cell is NOT a wall
//    if (map_pacman[next_y][next_x] != '#')
//    {
//        // Erase old position
//        int old_x = x[*tail];
//        int old_y = y[*tail];
//        LCD_FillRect(old_x * PACMAN_CELL_SIZE,
//                     old_y * PACMAN_CELL_SIZE,
//                     PACMAN_CELL_SIZE,
//                     PACMAN_CELL_SIZE,
//                     BG_COLOR);
//
//        // Advance tail
//        *tail = (*tail + 1) % 1000;
//
//        // Update head
//        int new_head = (*head + 1) % 1000;
//        x[new_head] = next_x;
//        y[new_head] = next_y;
//        *head = new_head;
//
//        // Draw new Pac-Man position
//        LCD_FillRect(next_x * PACMAN_CELL_SIZE,
//                     next_y * PACMAN_CELL_SIZE,
//                     PACMAN_CELL_SIZE,
//                     PACMAN_CELL_SIZE,
//                     PACMAN_COLOR);
//    }
//}
//
///**
// * @brief Build Pac-Man walls and draw them on LCD
// */
//void draw_pacman_map(int ROWS, int COLS, char map_pacman[ROWS][COLS])
//{
//    // Clear entire screen
//    LCD_Clear(BG_COLOR);
//
//    for (int j = 0; j < ROWS; j++)
//    {
//        for (int i = 0; i < COLS; i++)
//        {
//            // Default is empty cell
//            char cell_char = '.';
//
//            // Place walls (#) at certain spots
//            // Same logic you used before
//            if ((j == 0  && i == 8) ||
//                (j == 1  && (i == 1 || i == 2 || ... )) ||
//                ...
//                (j == 17 && (i == 1 || i == 2 || ... )))
//            {
//                cell_char = '#';
//            }
//
//            map_pacman[j][i] = cell_char;
//
//            if (cell_char == '#')
//            {
//                // Draw a wall
//                LCD_FillRect(i * PACMAN_CELL_SIZE,
//                             j * PACMAN_CELL_SIZE,
//                             PACMAN_CELL_SIZE,
//                             PACMAN_CELL_SIZE,
//                             WALL_COLOR);
//            }
//            else
//            {
//                // Draw empty background or a small dot
//                LCD_FillRect(i * PACMAN_CELL_SIZE,
//                             j * PACMAN_CELL_SIZE,
//                             PACMAN_CELL_SIZE,
//                             PACMAN_CELL_SIZE,
//                             BG_COLOR);
//
//                // Optionally draw a small dot
//                // e.g. LCD_Drawpixel(..., DOT_COLOR);
//            }
//        }
//    }
//}
