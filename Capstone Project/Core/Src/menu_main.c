#include "main.h"
#include "menu_main.h"
#include "lcd_driver.h"
#include "input.h"

#define line_height 20
void displayMenu() {
	LCD_Clear(BLACK);  // Clear the screen

	// Always display the header
	LCD_DrawString(50, 3 * line_height, "Welcome", WHITE, BLACK, 3);
	LCD_DrawString(50, 6 * line_height, "Select game", WHITE, BLACK, 2);
	LCD_DrawString(50, 7 * line_height, "Snake", WHITE, BLACK, 2);
	LCD_DrawString(50, 8 * line_height, "Pacman", WHITE, BLACK, 2);
	LCD_DrawString(50, 9 * line_height, "Tetris #", WHITE, BLACK, 2);
	int input = -1;
	int game_choice = 0;
	while (1) {
		for (int i = 0; i < 3; i += 1) {
			input = get_input();
			HAL_Delay(50);
		}
		if (input == 1) {
			play_snake();
			break;
		}
		if (input == 2) { // up logic
			if (game_choice != 0) {
				game_choice = (game_choice - 1) % 2;
			} else {
				game_choice = 2;
			}
		} else if (input == 5) { // down logic
			game_choice = (game_choice + 1) % 3;
		}

		if (game_choice == 0) {
			LCD_DrawString(140, 7 * line_height, " >", WHITE, BLACK, 2);
			clear_area(140, 8 * line_height, 170, 200, BLACK);
			print("0");
		} else if (game_choice == 1) {
			LCD_DrawString(140, 8 * line_height, " >", WHITE, BLACK, 2);
			clear_area(140, 7 * line_height, 170, 8 * line_height - 1, BLACK);
			clear_area(140, 9 * line_height, 170, 10 * line_height - 1, BLACK);
			print("1");
		} else if (game_choice == 2) {
			LCD_DrawString(140, 9 * line_height, " >", WHITE, BLACK, 2);
			clear_area(140, 7 * line_height, 170, 9 * line_height - 1, BLACK);
			print("2");
		}
		HAL_Delay(100);
	}

}
