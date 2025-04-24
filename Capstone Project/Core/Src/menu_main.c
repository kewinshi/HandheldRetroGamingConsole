#include "main.h"
#include "menu_main.h"
#include "lcd_driver.h"
#include "input.h"
#include "snake.h"
#include "tetris.h"
#include "pacman.h"
#include "space_invaders.h"
#include "pong.h"
#include "high_scores.h"
#include "buzzer_driver.h"

#define LINE_HEIGHT 20

void displayStartup() {
    LCD_Init();
    HAL_Delay(100);
    LCD_Clear(BLACK);
    LCD_DrawAMLogo();
    caneck_caneck();
}

// Display a submenu for the selected game, matching main menu drawing logic
static void displaySubMenu(uint8_t game_type) {
    LCD_Clear(BLACK);
    initStarField();

    // Game-specific details
    const char *game_name;
    uint16_t text_color;
    switch (game_type) {
        case 0: // Snake
            game_name = "SNAKE";
            text_color = LIGHT_GREEN;
            break;
        case 1: // Pacman
            game_name = "PACMAN";
            text_color = BLUE;
            break;
        case 2: // Tetris
            game_name = "TETRIS";
            text_color = RED;
            break;
        case 3: // Space Invaders
            game_name = "SPACE INVADERS";
            text_color = CYAN;
            break;
        case 4: // Pong
            game_name = "PONG";
            text_color = YELLOW;
            break;
        default:
            game_name = "UNKNOWN";
            text_color = WHITE;
            break;
    }

    int input = -1;
    int prev_choice = -1;
    int choice = 0;

    // Draw initial submenu state
    LCD_DrawString(50, 2 * LINE_HEIGHT, game_name, text_color, BLACK, 2);
    LCD_DrawString(50, 5 * LINE_HEIGHT, "Start", WHITE, BLACK, 1.5);
    LCD_DrawString(50, 7 * LINE_HEIGHT, "How to Play", WHITE, BLACK, 1.5);
    LCD_DrawString(50, 9 * LINE_HEIGHT, "Quit", WHITE, BLACK, 1.5);
    LCD_DrawImage(110, 4.7 * LINE_HEIGHT, 1, game_type); /* Initial logo at Start */

    while (1) {
        // Handle input
        if (input == 1) { // Select button
            buzz(50);
            if (choice == 0) { // Start
                LCD_Clear(BLACK);
                switch (game_type) {
                    case 0: play_snake(); break;
                    case 1: play_pacman(); break;
                    case 2: play_tetris(); break;
                    case 3: play_space_invaders(); break;
                    case 4: play_pong(); break;
                }
                // After game ends, return to submenu
                LCD_Clear(BLACK);
                initStarField();
                LCD_DrawString(50, 2 * LINE_HEIGHT, game_name, text_color, BLACK, 2);
                LCD_DrawString(50, 5 * LINE_HEIGHT, "Start", WHITE, BLACK, 1.5);
                LCD_DrawString(50, 7 * LINE_HEIGHT, "How to Play", WHITE, BLACK, 1.5);
                LCD_DrawString(50, 9 * LINE_HEIGHT, "Quit", WHITE, BLACK, 1.5);
                LCD_DrawImage(110, 4.7 * LINE_HEIGHT, 1, game_type); /* Redraw logo at Start */
                prev_choice = -1;
                choice = 0;
            } else if (choice == 1) { // How to Play
                LCD_Clear(BLACK);
                initStarField();
                LCD_DrawString(50, 2 * LINE_HEIGHT, "HOW TO PLAY", text_color, BLACK, 2);
                // Display game-specific instructions
                switch (game_type) {
                    case 0: // Snake
                        LCD_DrawString(50, 5 * LINE_HEIGHT, "Use Up/Down/Left/Right", WHITE, BLACK, 1);
                        LCD_DrawString(50, 6 * LINE_HEIGHT, "to move snake. Eat", WHITE, BLACK, 1);
                        LCD_DrawString(50, 7 * LINE_HEIGHT, "apples to grow.", WHITE, BLACK, 1);
                        break;
                    case 1: // Pacman
                        LCD_DrawString(50, 5 * LINE_HEIGHT, "Navigate maze with", WHITE, BLACK, 1);
                        LCD_DrawString(50, 6 * LINE_HEIGHT, "Up/Down/Left/Right.", WHITE, BLACK, 1);
                        LCD_DrawString(50, 7 * LINE_HEIGHT, "Eat dots, avoid ghosts.", WHITE, BLACK, 1);
                        break;
                    case 2: // Tetris
                        LCD_DrawString(50, 5 * LINE_HEIGHT, "Move blocks with", WHITE, BLACK, 1);
                        LCD_DrawString(50, 6 * LINE_HEIGHT, "Left/Right. Rotate with", WHITE, BLACK, 1);
                        LCD_DrawString(50, 7 * LINE_HEIGHT, "Select. Drop with Back.", WHITE, BLACK, 1);
                        break;
                    case 3: // Space Invaders
                        LCD_DrawString(50, 5 * LINE_HEIGHT, "Move ship with", WHITE, BLACK, 1);
                        LCD_DrawString(50, 6 * LINE_HEIGHT, "Left/Right. Shoot with", WHITE, BLACK, 1);
                        LCD_DrawString(50, 7 * LINE_HEIGHT, "Select to destroy aliens.", WHITE, BLACK, 1);
                        break;
                    case 4: // Pong
                        LCD_DrawString(50, 5 * LINE_HEIGHT, "Move paddle with", WHITE, BLACK, 1);
                        LCD_DrawString(50, 6 * LINE_HEIGHT, "Up/Down to hit ball.", WHITE, BLACK, 1);
                        LCD_DrawString(50, 7 * LINE_HEIGHT, "Score against opponent.", WHITE, BLACK, 1);
                        break;
                }
                LCD_DrawString(50, 9 * LINE_HEIGHT, "Press Select to return", WHITE, BLACK, 1);
                // Wait for select button to return to submenu
                input = -1;
                while (input != 1) {
                    for (int i = 0; i < 5; i++) {
                        input = get_input();
                        updateStarField();
                        HAL_Delay(20);
                    }
                }
                buzz(50);
                LCD_Clear(BLACK);
                initStarField();
                LCD_DrawString(50, 2 * LINE_HEIGHT, game_name, text_color, BLACK, 2);
                LCD_DrawString(50, 5 * LINE_HEIGHT, "Start", WHITE, BLACK, 1.5);
                LCD_DrawString(50, 7 * LINE_HEIGHT, "How to Play", WHITE, BLACK, 1.5);
                LCD_DrawString(50, 9 * LINE_HEIGHT, "Quit", WHITE, BLACK, 1.5);
                LCD_DrawImage(110, 4.7 * LINE_HEIGHT, 1, game_type); /* Redraw logo at Start */
                prev_choice = -1;
                choice = 0;
            } else if (choice == 2) { // Quit
                return; // Return to main menu
            }
        }

        // Navigation logic (matches main menu)
        if (input == 2) { // Up button
            buzz_low(15);
            if (choice != 0) {
                prev_choice = choice;
                choice = (choice - 1) % 3;
            } else {
                prev_choice = choice;
                choice = 2;
            }
        } else if (input == 5) { // Down button
            buzz(20);
            prev_choice = choice;
            choice = (choice + 1) % 3;
        }

        // Update menu visuals only when choice changes
        if (choice != prev_choice) {
            if (choice == 0) { // Start
                LCD_DrawImage(110, 4.7 * LINE_HEIGHT, 1, game_type); /* Logo at Start */
                clear_area(140, 6 * LINE_HEIGHT, 170, 10 * LINE_HEIGHT - 1, BLACK); /* Clear below */
            } else if (choice == 1) { // How to Play
                LCD_DrawImage(110, 6.7 * LINE_HEIGHT, 1, game_type); /* Logo at How to Play */
                clear_area(140, 4.5 * LINE_HEIGHT, 170, 6 * LINE_HEIGHT - 1, BLACK); /* Clear above */
                clear_area(140, 8 * LINE_HEIGHT, 170, 10 * LINE_HEIGHT - 1, BLACK); /* Clear below */
            } else if (choice == 2) { // Quit
                LCD_DrawImage(110, 8.7 * LINE_HEIGHT, 1, game_type); /* Logo at Quit */
                clear_area(140, 4.5 * LINE_HEIGHT, 170, 8 * LINE_HEIGHT - 1, BLACK); /* Clear above */
            }
            prev_choice = choice;
        }

        // Poll input and update star field
        for (int i = 0; i < 5; i++) {
            input = get_input();
            updateStarField();
            HAL_Delay(20);
        }
    }
}

void displayMenu() {
    LCD_Clear(BLACK);

    initStarField();
    LCD_DrawString(50, 2 * LINE_HEIGHT, "WELCOME", BLUE, BLACK, 3);
    LCD_DrawString(50, 5 * LINE_HEIGHT, "Snake", LIGHT_GREEN, BLACK, 1.5);
    LCD_DrawString(50, 7 * LINE_HEIGHT, "Pacman", BLUE, BLACK, 1.5);
    LCD_DrawString(50, 9 * LINE_HEIGHT, "Tetris", RED, BLACK, 1.5);
    LCD_DrawString(50, 11 * LINE_HEIGHT, "Space Invaders", CYAN, BLACK, 1.5);
    LCD_DrawString(50, 13 * LINE_HEIGHT, "Pong", YELLOW, BLACK, 1.5);
    LCD_DrawString(50, 15 * LINE_HEIGHT, "High Scores", WHITE, BLACK, 1.5);

    int input = -1;
    int prev_gamechoice = -1;
    int game_choice = 0;

    LCD_DrawImage(110, 4.7 * LINE_HEIGHT, 1, 0); /* Initial Snake logo */

    while (1) {
        if (input == 1) { /* Select button */
            buzz(50);
            if (game_choice == 5) { // High Scores: bypass submenu
                LCD_Clear(BLACK);
                play_high_scores();
            } else {
                displaySubMenu(game_choice);
            }
            // Redraw main menu after submenu or high scores exits
            LCD_Clear(BLACK);
            initStarField();
            LCD_DrawString(50, 2 * LINE_HEIGHT, "WELCOME", BLUE, BLACK, 3);
            LCD_DrawString(50, 5 * LINE_HEIGHT, "Snake", LIGHT_GREEN, BLACK, 1.5);
            LCD_DrawString(50, 7 * LINE_HEIGHT, "Pacman", BLUE, BLACK, 1.5);
            LCD_DrawString(50, 9 * LINE_HEIGHT, "Tetris", RED, BLACK, 1.5);
            LCD_DrawString(50, 11 * LINE_HEIGHT, "Space Invaders", CYAN, BLACK, 1.5);
            LCD_DrawString(50, 13 * LINE_HEIGHT, "Pong", YELLOW, BLACK, 1.5);
            LCD_DrawString(50, 15 * LINE_HEIGHT, "High Scores", WHITE, BLACK, 1.5);
            LCD_DrawImage(110, (4.7 + game_choice * 2) * LINE_HEIGHT, 1, game_choice);
            prev_gamechoice = -1;
        }

        /* Navigation logic */
        if (input == 2) { /* Up button */
            buzz_low(15);
            if (game_choice != 0) {
                prev_gamechoice = game_choice;
                game_choice = (game_choice - 1) % 6;
            } else {
                prev_gamechoice = game_choice;
                game_choice = 5;
            }
        } else if (input == 5) { /* Down button */
            buzz(20);
            prev_gamechoice = game_choice;
            game_choice = (game_choice + 1) % 6;
        }

        /* Update menu visuals */
        if (game_choice != prev_gamechoice) {
            if (game_choice == 0) {
                LCD_DrawImage(110, 4.7 * LINE_HEIGHT, 1, 0); /* Snake logo */
                clear_area(140, 6 * LINE_HEIGHT, 170, 16 * LINE_HEIGHT - 1, BLACK); /* Clear below */
            } else if (game_choice == 1) {
                LCD_DrawImage(110, 6.6 * LINE_HEIGHT, 1, 1); /* Pacman logo */
                clear_area(140, 4.5 * LINE_HEIGHT, 170, 6 * LINE_HEIGHT - 1, BLACK); /* Clear above */
                clear_area(140, 8 * LINE_HEIGHT, 170, 16 * LINE_HEIGHT - 1, BLACK); /* Clear below */
            } else if (game_choice == 2) {
                LCD_DrawImage(110, 8.7 * LINE_HEIGHT, 1, 2); /* Tetris logo */
                clear_area(140, 4.5 * LINE_HEIGHT, 170, 8 * LINE_HEIGHT - 1, BLACK); /* Clear above */
                clear_area(140, 10 * LINE_HEIGHT, 170, 16 * LINE_HEIGHT - 1, BLACK); /* Clear below */
            } else if (game_choice == 3) {
                LCD_DrawImage(110, 10.7 * LINE_HEIGHT, 1, 3); /* Space Invaders logo */
                clear_area(140, 4.5 * LINE_HEIGHT, 170, 10 * LINE_HEIGHT - 1, BLACK); /* Clear above */
                clear_area(140, 12 * LINE_HEIGHT, 170, 16 * LINE_HEIGHT - 1, BLACK); /* Clear below */
            } else if (game_choice == 4) {
                LCD_DrawImage(110, 12.7 * LINE_HEIGHT, 1, 4); /* Pong logo */
                clear_area(140, 4.5 * LINE_HEIGHT, 170, 12 * LINE_HEIGHT - 1, BLACK); /* Clear above */
                clear_area(140, 14 * LINE_HEIGHT, 170, 16 * LINE_HEIGHT - 1, BLACK); /* Clear below */
            } else if (game_choice == 5) {
                LCD_DrawImage(110, 14.7 * LINE_HEIGHT, 1, 5); /* High Scores logo */
                clear_area(140, 4.5 * LINE_HEIGHT, 170, 14 * LINE_HEIGHT - 1, BLACK); /* Clear above */
            }
            prev_gamechoice = game_choice;
        }

        /* Poll input and update star field */
        for (int i = 0; i < 5; i++) {
            input = get_input();
            updateStarField();
            HAL_Delay(20);
        }
    }
}
