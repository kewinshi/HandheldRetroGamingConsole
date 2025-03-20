#include "main.h"
#include "menu_main.h"
#include "lcd_driver.h"
#include "input.h"
#include "snake.h"
#include "tetris.h"

#define LINE_HEIGHT 20
void displayStartup() {
	LCD_Clear(BLACK);
	LCD_Init();
	HAL_Delay(100);
	LCD_DrawAMLogo();
	HAL_Delay(2000);

	// Display Offset Coordinate Test
//	LCD_Drawpixel(0, 0, GREEN);
//	LCD_Drawpixel(169, 0, RED);
//	LCD_Drawpixel(0, 319, BLUE);
//	LCD_Drawpixel(169, 319, GREEN);



}
void displayMenu() {
	LCD_Clear(BLACK);  // Clear the screen
//	drawSafeZones(); testing safe zones
	// Always display the header
	initStarField();
	LCD_DrawString(50, 3 * LINE_HEIGHT, "WELCOME", BLUE, BLACK, 3);
	LCD_DrawString(50, 7 * LINE_HEIGHT, "Snake", LIGHT_GREEN, BLACK, 1.5);
	LCD_DrawString(50, 9 * LINE_HEIGHT, "Pacman", BLUE, BLACK, 1.5);
	LCD_DrawString(50, 11 * LINE_HEIGHT, "Tetris", RED, BLACK, 1.5);
	int input = -1;
	int prev_gamechoice = -1;
	int game_choice = 0;

	// initialize snake logo
	LCD_DrawImage(110, 6.5 * LINE_HEIGHT, 1, 0);
	while (1) {
		if (input == 1 && game_choice == 0) {
			play_snake();
			break;
		} else if (input == 1 && game_choice == 2) {
			play_tetris();
			break;
		}
		if (input == 2) { // up logic
			if (game_choice != 0) {
				prev_gamechoice = game_choice;
				game_choice = (game_choice - 1) % 2;
			} else {
				prev_gamechoice = game_choice;
				game_choice = 2;
			}
		} else if (input == 5) { // down logic
			prev_gamechoice = game_choice;
			game_choice = (game_choice + 1) % 3;
		}

		if (game_choice == 0 && input != -1) {
			LCD_DrawImage(110, 6.7 * LINE_HEIGHT, 1, 0);
			clear_area(140, 8 * LINE_HEIGHT, 170, 14 * LINE_HEIGHT, BLACK);
		} else if (game_choice == 1 && input != -1) {
			LCD_DrawImage(110, 8.6 * LINE_HEIGHT, 1, 1);
			clear_area(140, 6.5 * LINE_HEIGHT, 170, 8 * LINE_HEIGHT - 1, BLACK);
			clear_area(140, 10.5 * LINE_HEIGHT, 170, 12 * LINE_HEIGHT - 1, BLACK);
		} else if (game_choice == 2 && input != -1) {
			clear_area(140, 6.5 * LINE_HEIGHT, 170, 10 * LINE_HEIGHT - 1, BLACK);
			LCD_DrawImage(110, 10.7 * LINE_HEIGHT, 1, 2);
		}
		for (int i = 0; i < 5; i++) {
			input = get_input();
			updateStarField();
			HAL_Delay(20);
		}
	}

}
