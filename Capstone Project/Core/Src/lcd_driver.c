#include "main.h"
#include "lcd_driver.h"
#include <stdlib.h>

//------------------------------------------------------------------------------
// LCD Low-Level Functions
//------------------------------------------------------------------------------

// Send a command to the LCD.
void LCD_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET); // Command mode
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

// Send data to the LCD.
void LCD_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET); // Data mode
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

// Same as LCD_WriteData but sends as batch
void LCD_WriteDataBlock(uint8_t *data, uint16_t length) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET); // Data mode
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, data, length, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

// Wrapper for LCD_WriteDataBlock but includes an offset of 35
void LCD_WriteBlockWithOffset(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
    LCD_SetWindow(x + 35, y, x + w - 1 + 35, y + h - 1);
    LCD_WriteDataBlock(data, w * h * 2); // 2 bytes per pixel
}

// Set the LCD display window.
void LCD_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    LCD_WriteCommand(0x2A);
    LCD_WriteData(x0 >> 8);
    LCD_WriteData(x0 & 0xFF);
    LCD_WriteData(x1 >> 8);
    LCD_WriteData(x1 & 0xFF);

    LCD_WriteCommand(0x2B);
    LCD_WriteData(y0 >> 8);
    LCD_WriteData(y0 & 0xFF);
    LCD_WriteData(y1 >> 8);
    LCD_WriteData(y1 & 0xFF);

    LCD_WriteCommand(0x2C);
}

// Initialize the LCD.
void LCD_Init(void)
{
    // Hardware Reset
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    // Basic initialization sequence
    LCD_WriteCommand(0x11);    // Sleep out
    HAL_Delay(120);

    LCD_WriteCommand(0x36);    // Memory Access Control
    LCD_WriteData(0x00);       // Normal orientation

    LCD_WriteCommand(0x3A);    // Interface Pixel Format
    LCD_WriteData(0x05);       // 16-bit color

    LCD_WriteCommand(0x21);    // Display Inversion On
    LCD_WriteCommand(0x13);    // Normal Display Mode On
    LCD_WriteCommand(0x29);    // Display On
    HAL_Delay(120);
}

// Clear the entire LCD screen with a given color.
void LCD_Clear(uint16_t color) {
    LCD_WriteCommand(0x2A); // Column Address Set
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData((LCD_WIDTH - 1) >> 8);
    LCD_WriteData((LCD_WIDTH - 1) & 0xFF);

    LCD_WriteCommand(0x2B); // Row Address Set
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData((LCD_HEIGHT - 1) >> 8);
    LCD_WriteData((LCD_HEIGHT - 1) & 0xFF);

    LCD_WriteCommand(0x2C); // Memory Write

    uint32_t num_pixels = LCD_WIDTH * LCD_HEIGHT;
    for (uint32_t i = 0; i < num_pixels; i++) {
        LCD_WriteData(color >> 8);
        LCD_WriteData(color & 0xFF);
    }
}

// Clear a rectangular area of the screen with a given color.
void clear_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    if (x1 >= LCD_WIDTH || y1 >= LCD_HEIGHT || x2 >= LCD_WIDTH || y2 >= LCD_HEIGHT) {
        return; // Out of bounds, do nothing
    }

    LCD_WriteCommand(0x2A); // Column Address Set
    LCD_WriteData(x1 >> 8);
    LCD_WriteData(x1 & 0xFF);
    LCD_WriteData(x2 >> 8);
    LCD_WriteData(x2 & 0xFF);

    LCD_WriteCommand(0x2B); // Row Address Set
    LCD_WriteData(y1 >> 8);
    LCD_WriteData(y1 & 0xFF);
    LCD_WriteData(y2 >> 8);
    LCD_WriteData(y2 & 0xFF);

    LCD_WriteCommand(0x2C); // Memory Write

    for (uint32_t i = 0; i < (x2 - x1 + 1) * (y2 - y1 + 1); i++) {
        LCD_WriteData(color >> 8);
        LCD_WriteData(color & 0xFF);
    }
}

// A testing function to fill the screen (for debugging).
void LCD_FillScreen(uint32_t color) {
    LCD_WriteCommand(0x2C); // RAM write command
    for (uint16_t i = 0; i < 320 * 170; i++) {
        LCD_WriteData(color >> 16);         // Red
        LCD_WriteData((color >> 8) & 0xFF);     // Green
        LCD_WriteData(color & 0xFF);            // Blue
    }
}

//------------------------------------------------------------------------------
// Font Definitions
//------------------------------------------------------------------------------

// Lowercase letters a-z
uint8_t font_a[7] = {0b00000,0b01110,0b00001,0b01111,0b10001,0b01111,0b00000};
uint8_t font_b[7] = {0b10000,0b10000,0b11110,0b10001,0b10001,0b11110,0b00000};
uint8_t font_c[7] = {0b00000,0b01111,0b10000,0b10000,0b10000,0b01111,0b00000};
uint8_t font_d[7] = {0b00001,0b00001,0b01111,0b10001,0b10001,0b01111,0b00000};
uint8_t font_e[7] = {0b00000,0b01110,0b10001,0b11111,0b10000,0b01111,0b00000};
uint8_t font_f[7] = {0b00111,0b01000,0b11110,0b01000,0b01000,0b01000,0b00000};
uint8_t font_g[7] = {0b00000,0b01111,0b10001,0b10001,0b01111,0b00001,0b11110};
uint8_t font_h[7] = {0b10000,0b10000,0b11110,0b10001,0b10001,0b10001,0b00000};
uint8_t font_i[7] = {0b00000,0b00100,0b00000,0b01100,0b00100,0b01110,0b00000};
uint8_t font_j[7] = {0b00000,0b00001,0b00001,0b00001,0b10001,0b01110,0b00000};
uint8_t font_k[7] = {0b10000,0b10010,0b10100,0b11000,0b10100,0b10010,0b00000};
uint8_t font_l[7] = {0b01100,0b00100,0b00100,0b00100,0b00100,0b01110,0b00000};
uint8_t font_m[7] = {0b00000,0b11010,0b10101,0b10101,0b10101,0b10101,0b00000};
uint8_t font_n[7] = {0b00000,0b11110,0b10001,0b10001,0b10001,0b10001,0b00000};
uint8_t font_o[7] = {0b00000,0b01110,0b10001,0b10001,0b10001,0b01110,0b00000};
uint8_t font_p[7] = {0b00000,0b11110,0b10001,0b11110,0b10000,0b10000,0b00000};
uint8_t font_q[7] = {0b00000,0b01111,0b10001,0b01111,0b00001,0b00001,0b00000};
uint8_t font_r[7] = {0b00000,0b11110,0b10001,0b10000,0b10000,0b10000,0b00000};
uint8_t font_s[7] = {0b00000,0b01111,0b10000,0b01110,0b00001,0b11110,0b00000};
uint8_t font_t[7] = {0b01000,0b11111,0b01000,0b01000,0b01000,0b00111,0b00000};
uint8_t font_u[7] = {0b00000,0b10001,0b10001,0b10001,0b10001,0b01111,0b00000};
uint8_t font_v[7] = {0b00000,0b10001,0b10001,0b10001,0b01010,0b00100,0b00000};
uint8_t font_w[7] = {0b00000,0b10001,0b10001,0b10101,0b10101,0b01010,0b00000};
uint8_t font_x[7] = {0b00000,0b10001,0b01010,0b00100,0b01010,0b10001,0b00000};
uint8_t font_y[7] = {0b00000,0b10001,0b10001,0b01111,0b00001,0b01110,0b00000};
uint8_t font_z[7] = {0b00000,0b11111,0b00010,0b00100,0b01000,0b11111,0b00000};

// Character '<' as a left-pointing arrow:
uint8_t font_less_than[7] = {
    0b00001,
    0b00010,
    0b00100,
    0b01000,
    0b00100,
    0b00010,
    0b00001
};



// Uppercase letters A-Z
uint8_t font_A[7] = {0b01110,0b10001,0b10001,0b11111,0b10001,0b10001,0b00000};
uint8_t font_B[7] = {0b11110,0b10001,0b11110,0b10001,0b10001,0b11110,0b00000};
uint8_t font_C[7] = {0b01111,0b10000,0b10000,0b10000,0b10000,0b01111,0b00000};
uint8_t font_D[7] = {0b11110,0b10001,0b10001,0b10001,0b10001,0b11110,0b00000};
uint8_t font_E[7] = {0b11111,0b10000,0b11110,0b10000,0b10000,0b11111,0b00000};
uint8_t font_F[7] = {0b11111,0b10000,0b11110,0b10000,0b10000,0b10000,0b00000};
uint8_t font_G[7] = {0b01111,0b10000,0b10011,0b10001,0b10001,0b01111,0b00000};
uint8_t font_H[7] = {0b10001,0b10001,0b11111,0b10001,0b10001,0b10001,0b00000};
uint8_t font_I[7] = {0b01110,0b00100,0b00100,0b00100,0b00100,0b01110,0b00000};
uint8_t font_J[7] = {0b00001,0b00001,0b00001,0b00001,0b10001,0b01110,0b00000};
uint8_t font_K[7] = {0b10001,0b10010,0b10100,0b11000,0b10100,0b10010,0b00000};
uint8_t font_L[7] = {0b10000,0b10000,0b10000,0b10000,0b10000,0b11111,0b00000};
uint8_t font_M[7] = {0b10001,0b11011,0b10101,0b10101,0b10001,0b10001,0b00000};
uint8_t font_N[7] = {0b10001,0b11001,0b10101,0b10011,0b10001,0b10001,0b00000};
uint8_t font_O[7] = {0b01110,0b10001,0b10001,0b10001,0b10001,0b01110,0b00000};
uint8_t font_P[7] = {0b11110,0b10001,0b11110,0b10000,0b10000,0b10000,0b00000};
uint8_t font_Q[7] = {0b01110,0b10001,0b10001,0b10101,0b10010,0b01101,0b00000};
uint8_t font_R[7] = {0b11110,0b10001,0b11110,0b10100,0b10010,0b10001,0b00000};
uint8_t font_S[7] = {0b01111,0b10000,0b01110,0b00001,0b00001,0b11110,0b00000};
uint8_t font_T[7] = {0b11111,0b00100,0b00100,0b00100,0b00100,0b00100,0b00000};
uint8_t font_U[7] = {0b10001,0b10001,0b10001,0b10001,0b10001,0b01110,0b00000};
uint8_t font_V[7] = {0b10001,0b10001,0b10001,0b10001,0b01010,0b00100,0b00000};
uint8_t font_W[7] = {0b10001,0b10001,0b10101,0b10101,0b11011,0b10001,0b00000};
uint8_t font_X[7] = {0b10001,0b01010,0b00100,0b00100,0b01010,0b10001,0b00000};
uint8_t font_Y[7] = {0b10001,0b10001,0b01110,0b00100,0b00100,0b00100,0b00000};
uint8_t font_Z[7] = {0b11111,0b00010,0b00100,0b01000,0b10000,0b11111,0b00000};

// Character '#' is not defined separately in this snippet; using font_block for '#'
uint8_t font_block[7] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b00000};

//------------------------------------------------------------------------------
// Constant Arrays for Fonts
//------------------------------------------------------------------------------

// Array for letters and special characters (lowercase a-z, uppercase A-Z, then '>' and '#')
// Total of 26 + 26 + 2 = 54 entries.
const uint8_t* font_data[54] = {
    font_a, font_b, font_c, font_d, font_e, font_f, font_g, font_h, font_i, font_j,
    font_k, font_l, font_m, font_n, font_o, font_p, font_q, font_r, font_s, font_t,
    font_u, font_v, font_w, font_x, font_y, font_z,
    font_A, font_B, font_C, font_D, font_E, font_F, font_G, font_H, font_I, font_J,
    font_K, font_L, font_M, font_N, font_O, font_P, font_Q, font_R, font_S, font_T,
    font_U, font_V, font_W, font_X, font_Y, font_Z,
    font_less_than, font_block
};

//---------------------
// Digit Fonts (0-9)
//---------------------
uint8_t font_0[7] = {0b00000,0b01110,0b10001,0b10001,0b10001,0b01110,0b00000};
uint8_t font_1[7] = {0b00000,0b00100,0b01100,0b00100,0b00100,0b00100,0b00000};
uint8_t font_2[7] = {0b00000,0b01110,0b10001,0b00001,0b00110,0b11111,0b00000};
uint8_t font_3[7] = {0b00000,0b01110,0b10001,0b00110,0b10001,0b01110,0b00000};
uint8_t font_4[7] = {0b00000,0b00010,0b00110,0b01010,0b11111,0b00010,0b00000};
uint8_t font_5[7] = {0b00000,0b11111,0b10000,0b11110,0b00001,0b11110,0b00000};
uint8_t font_6[7] = {0b00000,0b01110,0b10000,0b11110,0b10001,0b01110,0b00000};
uint8_t font_7[7] = {0b00000,0b11111,0b00001,0b00010,0b00100,0b00100,0b00000};
uint8_t font_8[7] = {0b00000,0b01110,0b10001,0b01110,0b10001,0b01110,0b00000};
uint8_t font_9[7] = {0b00000,0b01110,0b10001,0b01111,0b00001,0b01110,0b00000};

const uint8_t* font_digits[10] = {
    font_0, font_1, font_2, font_3, font_4,
    font_5, font_6, font_7, font_8, font_9
};

//------------------------------------------------------------------------------
// Character & String Drawing Functions
//------------------------------------------------------------------------------

// Draw a single character at (x, y) with foreground (color) and background (bgcolor) colors,
// scaled by 'scale'. Supports digits (0-9), letters (A-Z, a-z), and special characters '<' and '#'.
void LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor, uint8_t scale) {
    // Check if character is supported
    if (!((c >= '0' && c <= '9') ||
          (c >= 'A' && c <= 'Z') ||
          (c >= 'a' && c <= 'z') ||
          (c == '<') ||
          (c == '#'))) {
        return; // Unsupported character, do nothing
    }

    const uint8_t* char_data = 0;
    if (c >= '0' && c <= '9') {
        char_data = font_digits[c - '0'];
    } else if (c >= 'a' && c <= 'z') {
        char_data = font_data[c - 'a']; // Lowercase letters: indices 0–25
    } else if (c >= 'A' && c <= 'Z') {
        char_data = font_data[c - 'A' + 26]; // Uppercase letters: indices 26–51
    } else if (c == '<') {
        char_data = font_data[52];
    } else if (c == '#') {
        char_data = font_data[53];
    }

    // Draw each pixel as a scale x scale block.
    for (int row = 0; row < 7; row++) {
        uint8_t row_data = char_data[row];
        for (int col = 0; col < 5; col++) {
            if (row_data & (1 << (4 - col))) { // If pixel should be drawn
                for (int i = 0; i < scale; i++) {
                    for (int j = 0; j < scale; j++) {
                        LCD_Drawpixel(x + col * scale + i, y + row * scale + j, color);
                    }
                }
            } else {
                // Draw background pixel
                for (int i = 0; i < scale; i++) {
                    for (int j = 0; j < scale; j++) {
                        LCD_Drawpixel(x + col * scale + i, y + row * scale + j, bgcolor);
                    }
                }
            }
        }
    }
}

// Draw a string starting at (x, y) using LCD_DrawChar.
// Note: The x coordinate is adjusted by -35 as in your original code.
void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgcolor, uint8_t scale) {
    while (*str) {
        LCD_DrawChar(x - 35, y, *str, color, bgcolor, scale);
        x += (6 * scale); // Advance x-position (5 pixels + 1 pixel spacing)
        str++;
    }
}

//------------------------------------------------------------------------------
// Higher-Level Drawing Functions
//------------------------------------------------------------------------------

// Draw a single pixel on the display.
void LCD_Drawpixel(uint16_t x, uint16_t y, uint16_t color) {
    x = x + 35;  // Apply x offset as needed
    LCD_WriteCommand(0x2A); // Column address set
    LCD_WriteData(x >> 8);
    LCD_WriteData(x & 0xFF);

    LCD_WriteCommand(0x2B); // Row address set
    LCD_WriteData(y >> 8);
    LCD_WriteData(y & 0xFF);

    LCD_WriteCommand(0x2C); // Memory write
    LCD_WriteData(color >> 8);
    LCD_WriteData(color & 0xFF);
}

// Draw a 10x10 wall (filled rectangle) starting at (x, y).
void DrawWall(uint16_t x, uint16_t y, uint16_t color)
{
    // Draw the DARK_GREY outline (10x10 perimeter)
    for (int j = 0; j < 10; j++)
    {
        LCD_Drawpixel(x + j, y, DARK_GREY);          // Top edge
        LCD_Drawpixel(x + j, y + 9, DARK_GREY);      // Bottom edge
    }
    for (int i = 0; i < 10; i++)
    {
        LCD_Drawpixel(x, y + i, DARK_GREY);          // Left edge
        LCD_Drawpixel(x + 9, y + i, DARK_GREY);      // Right edge
    }

    // Fill the inner 8x8 area with the specified color
    for (int i = 1; i < 9; i++)
    {
        for (int j = 1; j < 9; j++)
        {
            LCD_Drawpixel(x + j, y + i, color);
        }
    }
}

// Draw a 14x15 wall (filled rectangle) with a DARK_GREY outline starting at (x, y).
void DrawWall14x15(uint16_t x, uint16_t y, uint16_t color)
{
    // Draw the DARK_GREY outline (14x15 perimeter)
    for (int j = 0; j < 15; j++)
    {
        LCD_Drawpixel(x + j, y, DARK_GREY);          // Top edge
        LCD_Drawpixel(x + j, y + 13, DARK_GREY);     // Bottom edge
    }
    for (int i = 0; i < 14; i++)
    {
        LCD_Drawpixel(x, y + i, DARK_GREY);          // Left edge
        LCD_Drawpixel(x + 14, y + i, DARK_GREY);     // Right edge
    }

    // Fill the inner 12x13 area with the specified color
    for (int i = 1; i < 13; i++)
    {
        for (int j = 1; j < 14; j++)
        {
            LCD_Drawpixel(x + j, y + i, color);
        }
    }
}

void DrawWall15x14(uint16_t x, uint16_t y, uint16_t color)
{
    // Draw the DARK_GREY outline (14x15 perimeter)
    for (int j = 0; j < 14; j++)
    {
        LCD_Drawpixel(x + j, y, DARK_GREY);          // Top edge
        LCD_Drawpixel(x + j, y + 13, DARK_GREY);     // Bottom edge
    }
    for (int i = 0; i < 15; i++)
    {
        LCD_Drawpixel(x, y + i, DARK_GREY);          // Left edge
        LCD_Drawpixel(x + 14, y + i, DARK_GREY);     // Right edge
    }

    // Fill the inner 12x13 area with the specified color
    for (int i = 1; i < 14; i++)
    {
        for (int j = 1; j < 13; j++)
        {
            LCD_Drawpixel(x + j, y + i, color);
        }
    }
}

// Draw a 14×14 filled rectangle (used for Tetris blocks)
void DrawTetrisBlock(uint16_t x, uint16_t y, uint16_t color) {
    uint16_t borderColor = (color == BLACK) ? BLACK : DARK_GREY;
    uint8_t data[14 * 14 * 2]; // 14x14 block, 2 bytes per pixel
    for (int i = 0; i < 14; i++) {
        for (int j = 0; j < 14; j++) {
            uint16_t c = (i == 0 || i == 13 || j == 0 || j == 13) ? borderColor : color;
            data[(i * 14 + j) * 2] = c >> 8;
            data[(i * 14 + j) * 2 + 1] = c & 0xFF;
        }
    }
    LCD_WriteBlockWithOffset(x, y, 14, 14, data);
}

void DrawTetrisBlockOutline(int x, int y, uint16_t color) {
    uint8_t data[14 * 14 * 2]; // 14x14 block, 2 bytes per pixel (16-bit color)
    uint16_t i, j;

    /* Initialize buffer: all pixels transparent (BLACK), then set outline */
    for (i = 0; i < 14 * 14 * 2; i += 2) {
        data[i] = BLACK >> 8;      /* High byte */
        data[i + 1] = BLACK & 0xFF; /* Low byte */
    }

    /* Top and bottom edges */
    for (j = 0; j < 14; j++) {
        data[j * 2] = color >> 8;           /* Top row */
        data[j * 2 + 1] = color & 0xFF;
        data[(13 * 14 + j) * 2] = color >> 8; /* Bottom row */
        data[(13 * 14 + j) * 2 + 1] = color & 0xFF;
    }

    /* Left and right edges (excluding corners to avoid overlap) */
    for (i = 1; i < 13; i++) {
        data[i * 14 * 2] = color >> 8;           /* Left column */
        data[i * 14 * 2 + 1] = color & 0xFF;
        data[(i * 14 + 13) * 2] = color >> 8;    /* Right column */
        data[(i * 14 + 13) * 2 + 1] = color & 0xFF;
    }

    /* Write the outline to the LCD */
    LCD_WriteBlockWithOffset((uint16_t)x, (uint16_t)y, 14, 14, data);
}


// Draw a snake body segment as a 10x10 square with a DARK_GREY border and gradient fill.
// If is_head is 1, add larger red eyes (2x2 blocks) based on direction (xdir, ydir).
void DrawSnakeBody(uint16_t x, uint16_t y, uint16_t color, int is_head, int xdir, int ydir)
{
    uint16_t light_color = LIGHT_GREEN;  // Center of gradient
    uint16_t dark_color = GREEN;         // Edges of gradient

    // Draw a 10x10 block
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int dx = j - 5;  // Center at (5,5)
            int dy = i - 5;
            if (i == 0 || i == 9 || j == 0 || j == 9)
            {
                // Draw DARK_GREY border
                LCD_Drawpixel(x + dx, y + dy, DARK_GREY);
            }
            else if (is_head && xdir == 1 &&  // Right direction
                     ((dx >= 2 && dx <= 3 && dy >= -2 && dy <= -1) ||  // Top eye (2x2)
                      (dx >= 2 && dx <= 3 && dy >= 1 && dy <= 2)))     // Bottom eye (2x2)
            {
                // Larger red eyes for head moving right
                LCD_Drawpixel(x + dx, y + dy, RED);
            }
            else if (is_head && xdir == -1 &&  // Left direction
                     ((dx >= -3 && dx <= -2 && dy >= -2 && dy <= -1) ||  // Top eye (2x2)
                      (dx >= -3 && dx <= -2 && dy >= 1 && dy <= 2)))     // Bottom eye (2x2)
            {
                // Larger red eyes for head moving left
                LCD_Drawpixel(x + dx, y + dy, RED);
            }
            else if (is_head && ydir == -1 &&  // Up direction
                     ((dx >= -3 && dx <= -2 && dy >= -3 && dy <= -2) ||  // Left eye (2x2)
                      (dx >= 1 && dx <= 2 && dy >= -3 && dy <= -2)))     // Right eye (2x2)
            {
                // Larger red eyes for head moving up
                LCD_Drawpixel(x + dx, y + dy, RED);
            }
            else if (is_head && ydir == 1 &&  // Down direction
                     ((dx >= -3 && dx <= -2 && dy >= 1 && dy <= 2) ||  // Left eye (2x2)
                      (dx >= 1 && dx <= 2 && dy >= 1 && dy <= 2)))     // Right eye (2x2)
            {
                // Larger red eyes for head moving down
                LCD_Drawpixel(x + dx, y + dy, RED);
            }
            else
            {
                // Gradient fill: lighter in center, darker near edges
                if (abs(dx) + abs(dy) < 4)  // Center area
                {
                    LCD_Drawpixel(x + dx, y + dy, light_color);
                }
                else  // Near edges
                {
                    LCD_Drawpixel(x + dx, y + dy, dark_color);
                }
            }
        }
    }
}

// Draw an apple as a filled circle (radius 5).
void DrawApple(uint16_t cx, uint16_t cy, uint16_t color) {
    int radius = 4;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                LCD_Drawpixel(cx + dx, cy + dy, color);
            }
        }
    }
}

// Draw a filled rectangle starting at (x,y) of width w and height h.
void LCD_FillRect(int x, int y, int w, int h, uint16_t color) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            LCD_Drawpixel(x + j, y + i, color);
        }
    }
}

// Draw a line from (x0,y0) to (x1,y1) using Bresenham's algorithm.
void LCD_DrawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        LCD_Drawpixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = err << 1;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}


const uint8_t pixel_array_logo[85][85] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};
void LCD_DrawAMLogo(void) {
    uint16_t startX = 0;  // Starting position on the screen
    uint16_t startY = 80;
    uint8_t scale = 2;    // Scale factor (increase for a bigger image)

    for (int y = 0; y < 85; y++) {
        for (int x = 0; x < 85; x++) {
            if (pixel_array_logo[y][x] == 1) {  // If pixel is maroon, draw it
                // Draw a larger block of pixels instead of a single pixel
                for (int i = 0; i < scale; i++) {
                    for (int j = 0; j < scale; j++) {
                        LCD_Drawpixel(startX + x * scale + i, startY + y * scale + j, MAROON);
                    }
                }
            }
        }
    }
}

static const uint8_t pixel_array_snake[16][16] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 5, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 5, 1, 1, 4, 1, 4, 1, 5, 0, 0},
    {0, 0, 0, 0, 0, 5, 1, 1, 2, 2, 1, 1, 1, 5, 0, 0},
    {0, 0, 0, 0, 5, 1, 1, 3, 5, 5, 3, 2, 4, 4, 5, 0},
    {0, 0, 0, 0, 5, 1, 1, 5, 0, 0, 5, 5, 5, 5, 4, 5},
    {0, 0, 0, 5, 5, 1, 1, 2, 5, 5, 0, 0, 0, 0, 5, 0},
    {0, 0, 5, 3, 5, 3, 1, 1, 1, 1, 5, 5, 5, 5, 0, 0},
    {0, 5, 2, 5, 2, 5, 3, 2, 2, 2, 1, 3, 5, 3, 5, 0},
    {5, 2, 1, 5, 1, 1, 5, 5, 5, 5, 1, 2, 5, 1, 2, 5},
    {5, 2, 1, 5, 3, 1, 1, 1, 1, 1, 1, 3, 5, 1, 2, 5},
    {5, 2, 1, 1, 5, 3, 2, 2, 2, 2, 3, 5, 1, 1, 2, 5},
    {5, 3, 2, 1, 1, 5, 5, 5, 5, 5, 5, 1, 1, 2, 3, 5},
    {0, 5, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 5, 0},
    {0, 0, 5, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 5, 0, 0},
    {0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0}
};

static const uint8_t pixel_array_pacman[16][16] = {
    {0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0},
    {0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0},
    {0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0},
    {0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6},
    {0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0},
    {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0},
    {0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0},
    {0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0}
};

uint8_t pixel_array_tetris[16][16] = {
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 7, 7, 7, 7, 7, 7, 5, 5, 7, 7, 7, 7, 7, 7, 5},
    {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}
};


// In space_invaders.c, after existing arrays
static const uint8_t pixel_array_space_invader[16][16] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // Padding row
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1},
    {0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
    {0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0},
    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  // Padding row
};

static const uint8_t pixel_array_pong[16][16] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0},
    {0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0},
    {0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0},
    {0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0},
    {0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0},
    {0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0},
    {0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0},
    {0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0},
    {0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0},
    {0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0},
    {0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0},
    {0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static const uint8_t pixel_array_high_scores[16][16] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void LCD_DrawImage(uint16_t startX, uint16_t startY, uint8_t scale, uint8_t type) {
    // Map pixel values (0-9) to corresponding colors
    uint16_t colors[] = {
        BLACK,        // 0
        LIGHT_GREEN,  // 1
        LIGHT_GREEN,  // 2
        LIGHT_GREEN,  // 3
        RED,          // 4
        DARK_GREY,    // 5
        YELLOW,       // 6
        RED,          // 7
        WHITE,        // 8
        WHITE       // 9 (for High Scores '<' icon)
    };

    // Select the pixel array based on type
    uint8_t (*pixelArray)[16] = NULL;
    switch (type) {
        case 0:
            pixelArray = pixel_array_snake;
            break;
        case 1:
            pixelArray = pixel_array_pacman;
            break;
        case 2:
            pixelArray = pixel_array_tetris;
            break;
        case 3:
            pixelArray = pixel_array_space_invader;
            break;
        case 4:
            pixelArray = pixel_array_pong;
            break;
        case 5:
            pixelArray = pixel_array_high_scores;
            break;
        default:
            pixelArray = pixel_array_snake;  // Fallback
            break;
    }

    // Calculate the total size of the scaled image
    uint16_t scaledWidth = 16 * scale;
    uint16_t scaledHeight = 16 * scale;
    uint16_t bufferSize = scaledWidth * scaledHeight * 2; // 2 bytes per pixel (16-bit color)
    uint8_t *buffer = (uint8_t *)malloc(bufferSize); // Dynamically allocate buffer

    if (buffer == NULL) {
        return; // Memory allocation failed, exit silently
    }

    // Fill the buffer with scaled pixel data
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            uint8_t pixelValue = pixelArray[y][x];
            uint16_t color = colors[pixelValue];
            // Scale each pixel into a block
            for (int dy = 0; dy < scale; dy++) {
                for (int dx = 0; dx < scale; dx++) {
                    int index = ((y * scale + dy) * scaledWidth + (x * scale + dx)) * 2;
                    buffer[index] = color >> 8;      // High byte
                    buffer[index + 1] = color & 0xFF; // Low byte
                }
            }
        }
    }

    // Write the entire block to the LCD
    LCD_WriteBlockWithOffset(startX, startY, scaledWidth, scaledHeight, buffer);

    // Free the allocated buffer
    free(buffer);
}

Star stars[NUM_STARS];

static uint16_t get_safe_y(void) {
    // Pick a random safe zone
    int zone = rand() % NUM_SAFE_ZONES;
    uint16_t y_min = safe_zones[zone].y_min;
    uint16_t y_max = safe_zones[zone].y_max;
    // Return a random y within that zone
    return y_min + (rand() % (y_max - y_min + 1));
}

// Initialize the star field with random positions and speeds
void initStarField(void) {
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].x = rand() % LCD_WIDTH;
        stars[i].y = get_safe_y(); // Use safe y-position
        stars[i].speed = 1 + (rand() % 3); // Speed between 1 and 3
    }
}

// Update and redraw the star field
void updateStarField(void) {
    for (int i = 0; i < NUM_STARS; i++) {
        // Erase the star at its current position
        LCD_Drawpixel(stars[i].x, stars[i].y, BACKGROUND_COLOR);

        // Update the star's horizontal position
        stars[i].x += stars[i].speed;

        // Wrap around if the star goes off the right edge
        if (stars[i].x >= LCD_WIDTH) {
            stars[i].x = 0;
            stars[i].y = get_safe_y(); // Reset to a safe y-position
        }

        // Draw the star at its new position
        LCD_Drawpixel(stars[i].x, stars[i].y, STAR_COLOR);
    }
}

void drawSafeZones(void) {
    for (int i = 0; i < NUM_SAFE_ZONES; i++) {
        uint16_t y_min = safe_zones[i].y_min;
        uint16_t y_max = safe_zones[i].y_max;
        for (uint16_t y = y_min; y <= y_max; y++) {
            for (uint16_t x = 0; x < LCD_WIDTH; x++) {
                LCD_Drawpixel(x, y, BLUE);
            }
        }
    }
}




