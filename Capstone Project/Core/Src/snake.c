#include "main.h"
#include "snake.h"
#include "lcd_driver.h"
#include "lcd_config.h"
#include "input.h"

#include <stdio.h>
#include <time.h>

void play_snake() {
	draw_snake_map();
	HAL_Delay(100);

	srand(time(NULL));
	int input = 3;
	int xdir = 1;
	int ydir = 0;

	int snake_x = (COLS - 2) / 2;
	int snake_y = (ROWS - 2) / 2;
	map[snake_x][snake_y] = 1;

	int apple_x = rand() % COLS;
	int apple_y = rand() % ROWS;
	while (input != 0)
	{
		for (int i = 0; i < 10; i += 1) {
			/* USER CODE BEGIN 3 */
			input = get_input();
			if (input == 2 && ydir != 1) { // up
				xdir = 0;
				ydir = -1;
//				print("up\n");
			}
			else if (input == 3 && xdir != 1) { // left
				xdir = -1;
				ydir = 0;
//				print("right\n");
			}
			else if (input == 4 && xdir != -1) { // right
				xdir = 1;
				ydir = 0;
//				print("left\n");
			}
			else if (input == 5 && ydir != -1) { // down
				xdir = 0;
				ydir = 1;
//				print("down\n");
			}
			HAL_Delay(20);
		}

//		if (1) {
//			apple_x = rand() % (COLS - 2) + 1;
//			apple_y = rand() % (ROWS - 2) + 1;
//		}

		// 10 offset pixels due to 10x10 asset size and shift by five due to driver drawing from center
		DrawApple(apple_x * 10 + 5, apple_y * 10 + 5, RED);
		move_snake(xdir, ydir);
	}
}
void draw_snake_map() {
//	LCD_Clear(BLACK);

    // Define the border colors using lcd_config definitions.
    uint16_t color = WHITE;   // White for the block pixels

    // Iterate through each cell in the grid.
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            // Draw the border if the cell is in the first/last row or column.
            if (row == 0 || row == ROWS - 1 || col == 0 || col == COLS - 1) {
                // Calculate the top-left pixel coordinate for this cell.
                // The drawn character occupies 6*scale pixels in width and 7*scale pixels in height.
                uint16_t x = col * 10;
                uint16_t y = row * 10;

                // Draw the block character using '#' (which maps to font_block in your font_data array)
                DrawWall(x, y, color);
            }
        }
    }
}

// This function moves the snake one step in the current direction.
// It calculates the new head position, draws the new head segment,
// erases the tail segment, and then updates the head and tail indices.
void move_snake(int xdir, int ydir) {
    // Calculate new head coordinates based on current head and direction
    int new_x = snake_x + xdir;
    int new_y = snake_y + ydir;

    // (Optional) Clamp or wrap the snake within the playable area.
    // Here we assume the playable area is within 1..(COLS-2) and 1..(ROWS-2)
    if(new_x < 1) new_x = 1;
    if(new_x > COLS - 2) new_x = COLS - 2;
    if(new_y < 1) new_y = 1;
    if(new_y > ROWS - 2) new_y = ROWS - 2;

    // Advance the head
    map[new_y][new_x] = 1;

    // Draw the new head segment.
    // The assets are 10x10 pixels; drawing from the center means adding 5
    DrawSnakeBody(new_x * 10 + 5, new_y * 10 + 5, GREEN);
    snake_x = new_x;
    snake_y = new_y;


}

