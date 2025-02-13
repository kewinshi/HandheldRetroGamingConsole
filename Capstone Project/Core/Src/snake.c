#include "main.h"
#include "snake.h"
#include "lcd_driver.h"
#include "lcd_config.h"
#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global snake body and its length.
Point snake_body[MAX_SNAKE_LENGTH];
int snake_length = 0;
int top_length = 0;

void play_snake() {
    draw_snake_map();
    HAL_Delay(100);

    srand(time(NULL));
    int input = 3;
    int xdir = 1;  // initial direction: right
    int ydir = 0;

    // Initialize snake with a length of 3.
    snake_length = 3;
    int center_x = (COLS - 2) / 2 + 1;
    int center_y = (ROWS - 2) / 2 + 1;
    // Set head at center.
    snake_body[0].x = center_x;
    snake_body[0].y = center_y;
    // The rest of the snake extends to the left (since we’re moving right).
    snake_body[1].x = center_x - 1;
    snake_body[1].y = center_y;
    snake_body[2].x = center_x - 2;
    snake_body[2].y = center_y;

    // Draw the initial snake.
    for (int i = 0; i < snake_length; i++) {
        DrawSnakeBody(snake_body[i].x * 10 + 5, snake_body[i].y * 10 + 5, GREEN);
    }

    // Generate the first apple ensuring it does not appear on the snake.
    int apple_x, apple_y;
    do {
        apple_x = rand() % (COLS - 2) + 1;
        apple_y = rand() % (ROWS - 2) + 1;
        int collision = 0;
        for (int i = 0; i < snake_length; i++) {
            if (snake_body[i].x == apple_x && snake_body[i].y == apple_y) {
                collision = 1;
                break;
            }
        }
        if (!collision) break;
    } while (1);
    DrawApple(apple_x * 10 + 5, apple_y * 10 + 5, RED);

    // Main game loop.
    while (input != 0) {
        // Poll input several times before moving.
        for (int i = 0; i < 10; i++) {
            input = get_input();
            if (input == 2 && ydir != 1) {       // up
                xdir = 0;
                ydir = -1;
            } else if (input == 3 && xdir != 1) { // left
                xdir = -1;
                ydir = 0;
            } else if (input == 4 && xdir != -1) { // right
                xdir = 1;
                ydir = 0;
            } else if (input == 5 && ydir != -1) { // down
                xdir = 0;
                ydir = 1;
            }
            HAL_Delay(20);
        }

        // Move the snake; if move_snake returns 0, game over.
        if (!move_snake(xdir, ydir, &apple_x, &apple_y)) {
            // Optionally, draw a "Game Over" message here.
        	if (snake_length > top_length) {
        		top_length = snake_length;
        		LCD_DrawString(50, 3 * 20, "HIGH SCORE!", WHITE, BLACK, 2.5);
        	} else {
        		LCD_DrawString(50, 3 * 20, "Game Over.", WHITE, BLACK, 2.5);
        	}

        	char score_str[20];
        	sprintf(score_str, "Score:%d", snake_length);
        	LCD_DrawString(50, 5 * 20, score_str, WHITE, BLACK, 2);
        	HAL_Delay(3000);
            break;
        }
    }
}

void draw_snake_map() {
    // Optionally clear the LCD:
     LCD_Clear(BLACK);

    uint16_t color = WHITE;   // White for border walls.
    // Draw the border walls.
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (row == 0 || row == ROWS - 1 || col == 0 || col == COLS - 1) {
                uint16_t x = col * 10;
                uint16_t y = row * 10;
                DrawWall(x, y, color); // weird shift going on where it sometimes requires x offset
            }
        }
    }
}

// Moves the snake one step in the given direction.
// Checks for collisions with walls and itself.
// Returns 1 if the snake is still alive, or 0 if a collision occurred (game over).
int move_snake(int xdir, int ydir, int *apple_x, int *apple_y) {
    // Compute the new head position.
    int new_x = snake_body[0].x + xdir;
    int new_y = snake_body[0].y + ydir;

    // Game over if the new head is out of bounds.
    if (new_x < 1 || new_x > COLS - 2 || new_y < 1 || new_y > ROWS - 2) {
        return 0;
    }

    // Check if the snake will eat the apple.
    int ate_apple = (new_x == *apple_x && new_y == *apple_y);

    // Check for collision with the snake's body.
    // If not eating an apple, the tail will be removed, so skip checking it.
    for (int i = 0; i < snake_length; i++) {
        if (!ate_apple && i == snake_length - 1)
            continue;
        if (snake_body[i].x == new_x && snake_body[i].y == new_y) {
            return 0;  // Collision with the body.
        }
    }

    // Shift the snake's segments.
    if (ate_apple) {
        // Increase the length: shift all segments one index back.
        for (int i = snake_length; i > 0; i--) {
            snake_body[i] = snake_body[i - 1];
        }
        snake_length++;
    } else {
        // Erase the tail from display.
        DrawSnakeBody(snake_body[snake_length - 1].x * 10 + 5,
                      snake_body[snake_length - 1].y * 10 + 5, BLACK);
        // Shift segments.
        for (int i = snake_length - 1; i > 0; i--) {
            snake_body[i] = snake_body[i - 1];
        }
    }

    // Insert the new head.
    snake_body[0].x = new_x;
    snake_body[0].y = new_y;
    // Draw the new head.
    DrawSnakeBody(new_x * 10 + 5, new_y * 10 + 5, GREEN);

    if (ate_apple) {
        // Generate a new apple in an empty cell.
        int new_apple_x, new_apple_y;
        do {
            new_apple_x = rand() % (COLS - 2) + 1;
            new_apple_y = rand() % (ROWS - 2) + 1;
            int collision = 0;
            for (int i = 0; i < snake_length; i++) {
                if (snake_body[i].x == new_apple_x && snake_body[i].y == new_apple_y) {
                    collision = 1;
                    break;
                }
            }
            if (!collision) break;
        } while (1);
        *apple_x = new_apple_x;
        *apple_y = new_apple_y;
        DrawApple(new_apple_x * 10 + 5, new_apple_y * 10 + 5, RED);
    }

    return 1;  // Snake is still alive.
}
