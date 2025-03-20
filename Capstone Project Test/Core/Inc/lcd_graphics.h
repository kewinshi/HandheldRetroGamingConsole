// In lcd_driver.h or a separate header file
#ifndef LCD_GRAPHICS_H
#define LCD_GRAPHICS_H

#include "lcd_driver.h"  // For LCD_Drawpixel, etc.

// Example 16-bit color macros (RGB565)
#define COLOR_BLACK  0x0000
#define COLOR_WHITE  0xFFFF
#define COLOR_RED    0xF800
#define COLOR_GREEN  0x07E0
#define COLOR_BLUE   0x001F
#define COLOR_YELLOW 0xFFE0

static inline void LCD_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    // Draws a rectangle of width w and height h at (x,y)
    for (uint16_t row = 0; row < h; row++) {
        for (uint16_t col = 0; col < w; col++) {
            LCD_Drawpixel(x + col, y + row, color);
        }
    }
}

#endif /* LCD_GRAPHICS_H */
