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
#define RED   0xFC00  // Adjusted for 6-bit red
#define GREEN 0x07E0  // Adjusted for 6-bit green
#define BLUE  0x001F  // Adjusted for 6-bit blue
#define GREY    0x8410
#define YELLOW  0xFFE0

// Menu navigation variables
typedef enum {
    MENU_ITEM_1,
    MENU_ITEM_2
} MenuItem;

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
void DrawSnakeBody(uint16_t cx, uint16_t cy, uint16_t color);
void DrawApple(uint16_t cx, uint16_t cy, uint16_t color);

#endif // LCD_HELPER_H
