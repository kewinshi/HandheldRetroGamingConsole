#include "main.h"
#include "high_scores.h"
#include "lcd_driver.h"
#include "input.h"
#include "buzzer_driver.h"
#include "snake.h"
#include "tetris.h"
#include <stdio.h>

#define LINE_HEIGHT 20

// Declare external high score variables from snake.c and tetris.c
extern int top_length;      // Snake high score
extern uint32_t top_score;  // Tetris high score

// Display the high scores screen
void play_high_scores(void) {
    LCD_Clear(BLACK);

    // Initialize star field for background
    initStarField();

    // Draw title
    LCD_DrawString(50, 2 * LINE_HEIGHT, "HIGH SCORES", ORANGE, BLACK, 2);

    // Draw game names and scores
    LCD_DrawString(50, 5 * LINE_HEIGHT, "Snake:", LIGHT_GREEN, BLACK, 1.5);
    char score_str[10];
    sprintf(score_str, "%d", top_length);
    LCD_DrawString(150, 5 * LINE_HEIGHT, score_str, WHITE, BLACK, 1.5);

    LCD_DrawString(50, 7 * LINE_HEIGHT, "Tetris:", RED, BLACK, 1.5);
    sprintf(score_str, "%lu", top_score);
    LCD_DrawString(150, 7 * LINE_HEIGHT, score_str, WHITE, BLACK, 1.5);

    int input = -1;
    while (1) {
        // Check for select button to exit
        if (input == 1) { /* Select button */
            buzz(50);
            break; // Return to main menu
        }

        // Poll input and update star field
        for (int i = 0; i < 5; i++) {
            input = get_input();
            updateStarField();
            HAL_Delay(20);
        }
    }
}

// Update high score for a game (0: Snake, 1: Tetris)
void update_high_score(uint8_t game_type, uint32_t score) {
    if (game_type == 0) { // Snake
        if (score > (uint32_t)top_length) {
            top_length = (int)score;
        }
    } else if (game_type == 1) { // Tetris
        if (score > top_score) {
            top_score = score;
        }
    }
}
