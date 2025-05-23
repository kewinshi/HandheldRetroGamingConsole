#include "pong.h"
#include "buzzer_driver.h"

/* Game state */
int left_paddle_y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
int right_paddle_y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
int ball_x = SCREEN_WIDTH / 2;
int ball_y = SCREEN_HEIGHT / 2;
int ball_dx = BALL_SPEED;
int ball_dy = 0;
uint32_t left_score = 0U;
uint32_t right_score = 0U;
int prev_left_paddle_y = -1;
int prev_right_paddle_y = -1;
int prev_ball_x = -1;
int prev_ball_y = -1;

/* Batch drawing buffer */
uint8_t paddle_buffer[PADDLE_WIDTH * PADDLE_HEIGHT * 2];
uint8_t ball_buffer[BALL_SIZE * BALL_SIZE * 2];

/* Initialize drawing buffers */
static void init_buffers(void) {
    for (int i = 0; i < PADDLE_WIDTH * PADDLE_HEIGHT * 2; i += 2) {
        paddle_buffer[i] = WHITE >> 8;
        paddle_buffer[i + 1] = WHITE & 0xFF;
    }
    for (int i = 0; i < BALL_SIZE * BALL_SIZE * 2; i += 2) {
        ball_buffer[i] = WHITE >> 8;
        ball_buffer[i + 1] = WHITE & 0xFF;
    }
}

/* Batch draw paddle, erase previous position */
static void batch_draw_paddle(int x, int y, int prev_y, uint16_t color) {
    if (prev_y != -1 && prev_y != y) {
        uint8_t black_buffer[PADDLE_WIDTH * PADDLE_HEIGHT * 2];
        for (int i = 0; i < PADDLE_WIDTH * PADDLE_HEIGHT * 2; i += 2) {
            black_buffer[i] = BLACK >> 8;
            black_buffer[i + 1] = BLACK & 0xFF;
        }
        LCD_WriteBlockWithOffset(x, prev_y, PADDLE_WIDTH, PADDLE_HEIGHT, black_buffer);
    }
    LCD_WriteBlockWithOffset(x, y, PADDLE_WIDTH, PADDLE_HEIGHT, paddle_buffer);
}

/* Batch draw ball, always erase previous position */
static void batch_draw_ball(int x, int y, int prev_x, int prev_y, uint16_t color) {
    if (prev_x != -1) {
        // Define the rectangle to erase
        int erase_x = prev_x;
        int erase_y = prev_y;
        int erase_w = BALL_SIZE;
        int erase_h = BALL_SIZE;

        // Clip to screen boundaries
        if (erase_x < 0) {
            erase_w += erase_x;  // Reduce width since erase_x is negative
            erase_x = 0;
        }
        if (erase_y < 0) {
            erase_h += erase_y;  // Reduce height
            erase_y = 0;
        }
        if (erase_x + erase_w > SCREEN_WIDTH) {
            erase_w = SCREEN_WIDTH - erase_x;
        }
        if (erase_y + erase_h > SCREEN_HEIGHT) {
            erase_h = SCREEN_HEIGHT - erase_y;
        }

        // Erase only if there's a valid area
        if (erase_w > 0 && erase_h > 0) {
            uint8_t black_buffer[erase_w * erase_h * 2];
            for (int i = 0; i < erase_w * erase_h * 2; i += 2) {
                black_buffer[i] = BLACK >> 8;
                black_buffer[i + 1] = BLACK & 0xFF;
            }
            LCD_WriteBlockWithOffset(erase_x, erase_y, erase_w, erase_h, black_buffer);
        }
    }
    // Draw the ball at the new position
    LCD_WriteBlockWithOffset(x, y, BALL_SIZE, BALL_SIZE, ball_buffer);
}

/* AI for right paddle - Stronger */
static void update_ai_paddle(void) {
    if (ball_y > right_paddle_y + PADDLE_HEIGHT / 2) {
        right_paddle_y += PADDLE_SPEED;
    } else if (ball_y < right_paddle_y + PADDLE_HEIGHT / 2) {
        right_paddle_y -= PADDLE_SPEED;
    }
    if (right_paddle_y < 0) right_paddle_y = 0;
    if (right_paddle_y > SCREEN_HEIGHT - PADDLE_HEIGHT) right_paddle_y = SCREEN_HEIGHT - PADDLE_HEIGHT;
}

/* Update ball position and collisions */
static void update_ball(void) {
    // Update ball position
    ball_x += ball_dx;
    ball_y += ball_dy;

    // Clamp ball position within screen boundaries
    if (ball_x < 0) {
        ball_x = 0;
    } else if (ball_x > SCREEN_WIDTH - BALL_SIZE) {
        ball_x = SCREEN_WIDTH - BALL_SIZE;
    }

    if (ball_y < 0) {
        ball_y = 0;
        ball_dy = BALL_SPEED;  // Bounce down
    } else if (ball_y > SCREEN_HEIGHT - BALL_SIZE) {
        ball_y = SCREEN_HEIGHT - BALL_SIZE;
        ball_dy = -BALL_SPEED;  // Bounce up
    }

    // Check collision with left paddle
    if (ball_dx < 0 && ball_x < PADDLE_WIDTH && ball_y + BALL_SIZE > left_paddle_y && ball_y < left_paddle_y + PADDLE_HEIGHT) {
        ball_dx = BALL_SPEED;  // Bounce right
        ball_dy = (ball_y + BALL_SIZE / 2 - (left_paddle_y + PADDLE_HEIGHT / 2)) * 0.5;  // Adjust vertical speed
        ball_x = PADDLE_WIDTH;  // Move ball out of paddle
        buzz(20);  // Sound effect
    }

    // Check collision with right (AI) paddle
    if (ball_dx > 0 && ball_x + BALL_SIZE > SCREEN_WIDTH - PADDLE_WIDTH && ball_y + BALL_SIZE > right_paddle_y && ball_y < right_paddle_y + PADDLE_HEIGHT) {
        ball_dx = -BALL_SPEED;  // Bounce left
        ball_dy = (ball_y + BALL_SIZE / 2 - (right_paddle_y + PADDLE_HEIGHT / 2)) * 0.5;  // Adjust vertical speed
        ball_x = SCREEN_WIDTH - PADDLE_WIDTH - BALL_SIZE;  // Move ball out of paddle
        buzz(20);  // Sound effect
    }

    // Handle scoring
    if (ball_x <= 0) {
        buzz_low(15);  // Sound effect for scoring
        right_score++;  // Right player scores
        reset_ball();   // Reset ball to center
    } else if (ball_x >= SCREEN_WIDTH - BALL_SIZE) {
        buzz(20);       // Sound effect for scoring
        left_score++;   // Left player scores
        reset_ball();   // Reset ball to center
    }
}

/* Draw scores using existing LCD_DrawString */
static void draw_score(void) {
    char score_str[20];
    sprintf(score_str, "%lu : %lu", (unsigned long)left_score, (unsigned long)right_score);
    LCD_DrawString(SCREEN_WIDTH / 2 - 20, 10, score_str, WHITE, BLACK, 1);
}

/* Reset ball to center */
static void reset_ball(void) {
    if (prev_ball_x != -1) {
        uint8_t black_buffer[BALL_SIZE * BALL_SIZE * 2];
        for (int i = 0; i < BALL_SIZE * BALL_SIZE * 2; i += 2) {
            black_buffer[i] = BLACK >> 8;
            black_buffer[i + 1] = BLACK & 0xFF;
        }
        LCD_WriteBlockWithOffset(prev_ball_x, prev_ball_y, BALL_SIZE, BALL_SIZE, black_buffer);
    }

    ball_x = SCREEN_WIDTH / 2;
    ball_y = SCREEN_HEIGHT / 2;
    ball_dx = (rand() % 2 == 0) ? BALL_SPEED : -BALL_SPEED;
    ball_dy = 0;
    prev_ball_x = -1;
    prev_ball_y = -1;
}

/* Main game loop with game over screen and score reset */
void play_pong(void) {
    // Reset scores at game start
    left_score = 0;
    right_score = 0;

    LCD_Clear(BLACK);
    init_buffers();
    reset_ball();

    while (1) {
        int input = get_input();
        if (input == INPUT_MOVE_UP && left_paddle_y > 0) {
            left_paddle_y -= PADDLE_SPEED;
        } else if (input == INPUT_MOVE_DOWN && left_paddle_y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
            left_paddle_y += PADDLE_SPEED;
        }

        update_ai_paddle();
        update_ball();

        if (left_score >= 5 || right_score >= 5) {
            if (left_score >= 5) {
                buzz(500);
                LCD_DrawString(50, 100, "You Win!", WHITE, BLACK, 2);
            } else {
                buzz_low(300);
                LCD_DrawString(50, 100, "You Lose!", WHITE, BLACK, 2);
            }
            HAL_Delay(2000); // Display for 2 seconds before exiting
            break;
        }

        batch_draw_paddle(0, left_paddle_y, prev_left_paddle_y, WHITE);
        batch_draw_paddle(SCREEN_WIDTH - PADDLE_WIDTH, right_paddle_y, prev_right_paddle_y, WHITE);
        batch_draw_ball(ball_x, ball_y, prev_ball_x, prev_ball_y, WHITE);
        draw_score();

        prev_left_paddle_y = left_paddle_y;
        prev_right_paddle_y = right_paddle_y;
        prev_ball_x = ball_x;
        prev_ball_y = ball_y;

        HAL_Delay(16);
    }
}
