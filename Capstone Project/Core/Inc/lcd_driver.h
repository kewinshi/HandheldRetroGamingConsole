#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>
#include "main.h"

extern SPI_HandleTypeDef hspi1;

// Constants
#define MENU_ITEMS     2
#define LCD_CS_Pin GPIO_PIN_6
#define LCD_CS_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_8
#define LCD_DC_GPIO_Port GPIOA
#define LCD_RST_Pin GPIO_PIN_9
#define LCD_RST_GPIO_Port GPIOA

#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED   0xFC00
#define BLUE  0x001F
#define GREY    0x8410
#define YELLOW  0xFFE0
#define MAROON 0x8000
#define LIGHT_GREEN 0x07E0
#define GREEN 0x07E0
#define DARK_GREEN 0x01E0
#define DARK_GREY 0x4208  // Darker than GREY but not full black
#define CYAN    0x07FF  // RGB565: (0, 63, 31)
#define PURPLE  0x801F  // RGB565: (16, 0, 31)
#define ORANGE  0xFC20  // RGB565: (31, 32, 0)

#define LCD_WIDTH    240
#define LCD_HEIGHT   320

// Menu navigation variables
typedef enum {
    MENU_ITEM_1,
    MENU_ITEM_2
} MenuItem;

// Structure for a star
typedef struct {
    int x;
    int y;
    int speed;  // Vertical speed (e.g., 1 to 3 pixels per update)
} Star;

// Safe y-ranges for stars
static const struct {
    uint16_t y_min;
    uint16_t y_max;
} safe_zones[] = {
    {0, 59},      // Above "WELCOME"
    {59 + 23, 125}, // Between "WELCOME" and "Snake"
	{150, 170}, // Between "Snake" and "Pac-Man"
	{190, 210},   // Between "Pac-Man and Tetris"
    {230, 255},    // Between "Tetris" and Space Invaders"
    {270,319} // Below "Space Invaders"
};
#define NUM_SAFE_ZONES (sizeof(safe_zones) / sizeof(safe_zones[0]))


extern MenuItem currentMenuItem;

// Menu navigation functions
void navigateMenuUp(void);
void navigateMenuDown(void);
void selectMenuItem(void);

// LCD control functions
void LCD_WriteCommand(uint8_t cmd);
void LCD_WriteData(uint8_t data);
void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_Init(void);
void LCD_Clear(uint16_t color);
void clear_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

// LCD drawing functions
void LCD_Drawpixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor, uint8_t scale);
void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgcolor, uint8_t scale);

// Snake Assets

void DrawWall(uint16_t x, uint16_t y, uint16_t color);
void DrawWall5(uint16_t x, uint16_t y, uint16_t color);
void DrawTetrisBlock(uint16_t x, uint16_t y, uint16_t color);
void DrawSnakeBody(uint16_t x, uint16_t y, uint16_t color, int is_head, int xdir, int ydir);
void DrawApple(uint16_t cx, uint16_t cy, uint16_t color);
void LCD_FillRect(int x, int y, int w, int h, uint16_t color);
void LCD_DrawLine(int x0, int y0, int x1, int y1, uint16_t color);
void LCD_DrawAMLogo(void);
void LCD_DrawImage(uint16_t startX, uint16_t startY, uint8_t scale, uint8_t type);
void initStarField(void);
void updateStarField(void);
void drawSafeZones(void);

#define NUM_STARS 10
#define STAR_COLOR WHITE
#define BACKGROUND_COLOR BLACK

// Declare the global star array as extern
extern Star stars[NUM_STARS];
#endif // LCD_HELPER_H
