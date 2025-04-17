#include "main.h"
#include "menu_main.h"
#include "lcd_driver.h"
#include "input.h"
#include "snake.h"
#include "tetris.h"
#include "pacman.h"
#include "space_invaders.h"
#include "pong.h"
#include "buzzer_driver.h"

#define LINE_HEIGHT 20

void displayStartup() {
	LCD_Init();
	HAL_Delay(100);
    LCD_Clear(BLACK);
    LCD_DrawAMLogo();
    caneck_caneck();
}

void displayMenu() {
    LCD_Clear(BLACK);

    initStarField();
    LCD_DrawString(50, 3 * LINE_HEIGHT, "WELCOME", BLUE, BLACK, 3);
    LCD_DrawString(50, 7 * LINE_HEIGHT, "Snake", LIGHT_GREEN, BLACK, 1.5);
    LCD_DrawString(50, 9 * LINE_HEIGHT, "Pacman", BLUE, BLACK, 1.5);
    LCD_DrawString(50, 11 * LINE_HEIGHT, "Tetris", RED, BLACK, 1.5);
    LCD_DrawString(50, 13 * LINE_HEIGHT, "Space Invaders", CYAN, BLACK, 1.5);
    LCD_DrawString(50, 15 * LINE_HEIGHT, "Pong", YELLOW, BLACK, 1.5);

    int input = -1;
    int prev_gamechoice = -1;
    int game_choice = 0;

    LCD_DrawImage(110, 6.7 * LINE_HEIGHT, 1, 0); /* Initial Snake logo */

    while (1) {
        if (input == 1) { /* Select button */
        	buzz(50);
            if (game_choice == 0) {
                play_snake();
                break;
            } else if (game_choice == 1) {
                play_pacman();
                break;
            } else if (game_choice == 2) {
                play_tetris();
                break;
            } else if (game_choice == 3) {
                play_space_invaders();
                break;
            } else if (game_choice == 4) {
                play_pong();
                break;
            }
        }

        /* Navigation logic */
        if (input == 2) { /* Up button */
        	buzz_low(15);
            if (game_choice != 0) {
                prev_gamechoice = game_choice;
                game_choice = (game_choice - 1) % 5;
            } else {
                prev_gamechoice = game_choice;
                game_choice = 4;
            }
        } else if (input == 5) { /* Down button */
        	buzz(20);
            prev_gamechoice = game_choice;
            game_choice = (game_choice + 1) % 5;
        }

        /* Update menu visuals */
        if (game_choice == 0 && input != -1) {
            LCD_DrawImage(110, 6.7 * LINE_HEIGHT, 1, 0); /* Snake logo */
            clear_area(140, 8 * LINE_HEIGHT, 170, 16 * LINE_HEIGHT - 1, BLACK); /* Clear below, including Pong */
        } else if (game_choice == 1 && input != -1) {
            LCD_DrawImage(110, 8.6 * LINE_HEIGHT, 1, 1); /* Pacman logo */
            clear_area(140, 6.5 * LINE_HEIGHT, 170, 8 * LINE_HEIGHT - 1, BLACK); /* Clear above */
            clear_area(140, 10 * LINE_HEIGHT, 170, 16 * LINE_HEIGHT - 1, BLACK); /* Clear below, including Pong */
        } else if (game_choice == 2 && input != -1) {
            LCD_DrawImage(110, 10.7 * LINE_HEIGHT, 1, 2); /* Tetris logo */
            clear_area(140, 6.5 * LINE_HEIGHT, 170, 10 * LINE_HEIGHT - 1, BLACK); /* Clear above */
            clear_area(140, 12 * LINE_HEIGHT, 170, 16 * LINE_HEIGHT - 1, BLACK); /* Clear below, including Pong */
        } else if (game_choice == 3 && input != -1) {
            LCD_DrawImage(110, 12.7 * LINE_HEIGHT, 1, 3); /* Space Invaders logo */
            clear_area(140, 6.5 * LINE_HEIGHT, 170, 12 * LINE_HEIGHT - 1, BLACK); /* Clear above */
            clear_area(140, 14 * LINE_HEIGHT, 170, 16 * LINE_HEIGHT - 1, BLACK); /* Clear Pong below */
        } else if (game_choice == 4 && input != -1) {
            LCD_DrawImage(110, 14.7 * LINE_HEIGHT, 1, 4); /* Pong logo */
            clear_area(140, 6.5 * LINE_HEIGHT, 170, 14 * LINE_HEIGHT - 1, BLACK); /* Clear all above */
        }

        /* Poll input and update star field */
        for (int i = 0; i < 5; i++) {
            input = get_input();
            updateStarField();
            HAL_Delay(20);
        }
    }
}
