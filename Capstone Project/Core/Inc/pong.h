#ifndef PONG_H
#define PONG_H

#include "main.h"
#include "lcd_driver.h"
#include "input.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Game constants */
#define SCREEN_WIDTH    170U
#define SCREEN_HEIGHT   320U
#define PADDLE_WIDTH    5U
#define PADDLE_HEIGHT   40U
#define BALL_SIZE       5U
#define PADDLE_SPEED    4
#define BALL_SPEED      3
#define INVALID_INPUT   (-1)
#define INPUT_MOVE_UP   (2)  /* Move paddle up */
#define INPUT_MOVE_DOWN (5)  /* Move paddle down */

/* Game state */
extern int left_paddle_y;
extern int right_paddle_y;
extern int ball_x;
extern int ball_y;
extern int ball_dx;
extern int ball_dy;
extern uint32_t left_score;
extern uint32_t right_score;
extern int prev_left_paddle_y;
extern int prev_right_paddle_y;
extern int prev_ball_x;
extern int prev_ball_y;

/* Batch drawing buffer */
extern uint8_t paddle_buffer[PADDLE_WIDTH * PADDLE_HEIGHT * 2];
extern uint8_t ball_buffer[BALL_SIZE * BALL_SIZE * 2];

/* Function prototypes */
void play_pong(void);
static void init_buffers(void);
static void batch_draw_paddle(int x, int y, int prev_y, uint16_t color);
static void batch_draw_ball(int x, int y, int prev_x, int prev_y, uint16_t color);
static void update_ai_paddle(void);
static void update_ball(void);
static void draw_score(void);
static void reset_ball(void);

#endif /* PONG_H */
