#include "main.h"
#include "lcd_driver.h"
#include "lcd_config.h"




//lcd commands
void LCD_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET); // Command mode
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

}
//lcd  send datas
void LCD_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET); // Data mode
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
//set the boundary of the lcd
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
//initialize the lcd
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
    HAL_Delay(120);           // Required delay

    LCD_WriteCommand(0x36);    // Memory Access Control
    LCD_WriteData(0x00);      // Normal orientation

    LCD_WriteCommand(0x3A);    // Interface Pixel Format
    LCD_WriteData(0x05);      // 16-bit color

    LCD_WriteCommand(0x21);    // Display Inversion On

    LCD_WriteCommand(0x13);    // Normal Display Mode On

    LCD_WriteCommand(0x29);    // Display On
    HAL_Delay(120);

}
//one of the main driver function, clears the lcd screen
void LCD_Clear(uint16_t color) {
    // 1. Set the Column Address Range (X-axis)
    LCD_WriteCommand(0x2A); // Column Address Set
    LCD_WriteData(0x00);     // Start column high byte (0)
    LCD_WriteData(0x00);     // Start column low byte (0)
    LCD_WriteData((LCD_WIDTH - 1) >> 8); // End column high byte
    LCD_WriteData((LCD_WIDTH - 1) & 0xFF); // End column low byte (169 for 170px width)

    // 2. Set the Row Address Range (Y-axis)
    LCD_WriteCommand(0x2B); // Row Address Set
    LCD_WriteData(0x00);     // Start row high byte (0)
    LCD_WriteData(0x00);     // Start row low byte (0)
    LCD_WriteData((LCD_HEIGHT - 1) >> 8); // End row high byte
    LCD_WriteData((LCD_HEIGHT - 1) & 0xFF); // End row low byte (319 for 320px height)

    // 3. Start Memory Write
    LCD_WriteCommand(0x2C); // Memory Write

    // 4. Calculate the total number of pixels to clear
    uint32_t num_pixels = LCD_WIDTH * LCD_HEIGHT; // 170 * 320 = 54,400 pixels

    // 5. Fill the screen with the specified color
    for (uint32_t i = 0; i < num_pixels; i++) {
        LCD_WriteData(color >> 8);   // Send high byte of the color
        LCD_WriteData(color & 0xFF); // Send low byte of the color
    }
}
void clear_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    // Ensure the coordinates are within bounds
    if (x1 >= LCD_WIDTH || y1 >= LCD_HEIGHT || x2 >= LCD_WIDTH || y2 >= LCD_HEIGHT) {
        return; // Out of bounds, do nothing
    }

    // Set the column address range
    LCD_WriteCommand(0x2A); // Column Address Set
    LCD_WriteData(x1 >> 8); // Start column high byte
    LCD_WriteData(x1 & 0xFF); // Start column low byte
    LCD_WriteData(x2 >> 8); // End column high byte
    LCD_WriteData(x2 & 0xFF); // End column low byte

    // Set the row address range
    LCD_WriteCommand(0x2B); // Row Address Set
    LCD_WriteData(y1 >> 8); // Start row high byte
    LCD_WriteData(y1 & 0xFF); // Start row low byte
    LCD_WriteData(y2 >> 8); // End row high byte
    LCD_WriteData(y2 & 0xFF); // End row low byte

    // Start writing to memory
    LCD_WriteCommand(0x2C); // Memory Write

    // Fill the specified area with the color
    for (uint32_t i = 0; i < (x2 - x1 + 1) * (y2 - y1 + 1); i++) {
        LCD_WriteData(color >> 8);    // Send high byte of the color
        LCD_WriteData(color & 0xFF); // Send low byte of the color
    }
}


//testing function
void LCD_FillScreen(uint32_t color) {
    LCD_WriteCommand(0x2C); // RAM write command
    for (uint16_t i = 0; i < 320 * 170; i++) {
        LCD_WriteData(color >> 16); // Red
        LCD_WriteData((color >> 8) & 0xFF); // Green
        LCD_WriteData(color & 0xFF); // Blue
    }
}
//characters a-z
    uint8_t font_a[7] = {
	    0b00000,
	    0b01110,
	    0b00001,
	    0b01111,
	    0b10001,
	    0b01111,
	    0b00000
	};

	uint8_t font_b[7] = {
	    0b10000,
	    0b10000,
	    0b11110,
	    0b10001,
	    0b10001,
	    0b11110,
	    0b00000
	};

	uint8_t font_c[7] = {
	    0b00000,
	    0b01111,
	    0b10000,
	    0b10000,
	    0b10000,
	    0b01111,
	    0b00000
	};

	uint8_t font_d[7] = {
	    0b00001,
	    0b00001,
	    0b01111,
	    0b10001,
	    0b10001,
	    0b01111,
	    0b00000
	};

	uint8_t font_e[7] = {
	    0b00000,
	    0b01110,
	    0b10001,
	    0b11111,
	    0b10000,
	    0b01111,
	    0b00000
	};

	uint8_t font_f[7] = {
	    0b00111,
	    0b01000,
	    0b11110,
	    0b01000,
	    0b01000,
	    0b01000,
	    0b00000
	};

	uint8_t font_g[7] = {
	    0b00000,
	    0b01111,
	    0b10001,
	    0b10001,
	    0b01111,
	    0b00001,
	    0b11110
	};

	uint8_t font_h[7] = {
	    0b10000,
	    0b10000,
	    0b11110,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b00000
	};

	uint8_t font_i[7] = {
	    0b00000,
	    0b00100,
	    0b00000,
	    0b01100,
	    0b00100,
	    0b01110,
	    0b00000
	};

	uint8_t font_j[7] = {
	    0b00000,
	    0b00001,
	    0b00001,
	    0b00001,
	    0b10001,
	    0b01110,
	    0b00000
	};

	uint8_t font_k[7] = {
	    0b10000,
	    0b10010,
	    0b10100,
	    0b11000,
	    0b10100,
	    0b10010,
	    0b00000
	};

	uint8_t font_l[7] = {
	    0b01100,
	    0b00100,
	    0b00100,
	    0b00100,
	    0b00100,
	    0b01110,
	    0b00000
	};

	uint8_t font_m[7] = {
	    0b00000,
	    0b11010,
	    0b10101,
	    0b10101,
	    0b10101,
	    0b10101,
	    0b00000
	};

	uint8_t font_n[7] = {
	    0b00000,
	    0b11110,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b00000
	};

	uint8_t font_o[7] = {
	    0b00000,
	    0b01110,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b01110,
	    0b00000
	};

	uint8_t font_p[7] = {
	    0b00000,
	    0b11110,
	    0b10001,
	    0b11110,
	    0b10000,
	    0b10000,
	    0b00000
	};

	uint8_t font_q[7] = {
	    0b00000,
	    0b01111,
	    0b10001,
	    0b01111,
	    0b00001,
	    0b00001,
	    0b00000
	};

	uint8_t font_r[7] = {
	    0b00000,
	    0b11110,
	    0b10001,
	    0b10000,
	    0b10000,
	    0b10000,
	    0b00000
	};

	uint8_t font_s[7] = {
	    0b00000,
	    0b01111,
	    0b10000,
	    0b01110,
	    0b00001,
	    0b11110,
	    0b00000
	};

	uint8_t font_t[7] = {
	    0b01000,
	    0b11111,
	    0b01000,
	    0b01000,
	    0b01000,
	    0b00111,
	    0b00000
	};

	uint8_t font_u[7] = {
	    0b00000,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b01111,
	    0b00000
	};

	uint8_t font_v[7] = {
	    0b00000,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b01010,
	    0b00100,
	    0b00000
	};

	uint8_t font_w[7] = {
	    0b00000,
	    0b10001,
	    0b10001,
	    0b10101,
	    0b10101,
	    0b01010,
	    0b00000
	};

	uint8_t font_x[7] = {
	    0b00000,
	    0b10001,
	    0b01010,
	    0b00100,
	    0b01010,
	    0b10001,
	    0b00000
	};

	uint8_t font_y[7] = {
	    0b00000,
	    0b10001,
	    0b10001,
	    0b01111,
	    0b00001,
	    0b01110,
	    0b00000
	};

	uint8_t font_z[7] = {
	    0b00000,
	    0b11111,
	    0b00010,
	    0b00100,
	    0b01000,
	    0b11111,
	    0b00000
	};
	// Character '>'
	uint8_t font_greater_than[7] = {
	    0b10000,
	    0b01000,
	    0b00100,
	    0b00010,
	    0b00100,
	    0b01000,
	    0b10000
	};
	uint8_t font_A[7] = {
	    0b01110,
	    0b10001,
	    0b10001,
	    0b11111,
	    0b10001,
	    0b10001,
	    0b00000
	};

	uint8_t font_B[7] = {
	    0b11110,
	    0b10001,
	    0b11110,
	    0b10001,
	    0b10001,
	    0b11110,
	    0b00000
	};

	uint8_t font_C[7] = {
	    0b01111,
	    0b10000,
	    0b10000,
	    0b10000,
	    0b10000,
	    0b01111,
	    0b00000
	};

	uint8_t font_D[7] = {
	    0b11110,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b11110,
	    0b00000
	};

	uint8_t font_E[7] = {
	    0b11111,
	    0b10000,
	    0b11110,
	    0b10000,
	    0b10000,
	    0b11111,
	    0b00000
	};

	uint8_t font_F[7] = {
	    0b11111,
	    0b10000,
	    0b11110,
	    0b10000,
	    0b10000,
	    0b10000,
	    0b00000
	};

	uint8_t font_G[7] = {
	    0b01111,
	    0b10000,
	    0b10011,
	    0b10001,
	    0b10001,
	    0b01111,
	    0b00000
	};

	uint8_t font_H[7] = {
	    0b10001,
	    0b10001,
	    0b11111,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b00000
	};

	uint8_t font_I[7] = {
	    0b01110,
	    0b00100,
	    0b00100,
	    0b00100,
	    0b00100,
	    0b01110,
	    0b00000
	};

	uint8_t font_J[7] = {
	    0b00001,
	    0b00001,
	    0b00001,
	    0b00001,
	    0b10001,
	    0b01110,
	    0b00000
	};

	uint8_t font_K[7] = {
	    0b10001,
	    0b10010,
	    0b10100,
	    0b11000,
	    0b10100,
	    0b10010,
	    0b00000
	};

	uint8_t font_L[7] = {
	    0b10000,
	    0b10000,
	    0b10000,
	    0b10000,
	    0b10000,
	    0b11111,
	    0b00000
	};

	uint8_t font_M[7] = {
	    0b10001,
	    0b11011,
	    0b10101,
	    0b10101,
	    0b10001,
	    0b10001,
	    0b00000
	};

	uint8_t font_N[7] = {
	    0b10001,
	    0b11001,
	    0b10101,
	    0b10011,
	    0b10001,
	    0b10001,
	    0b00000
	};

	uint8_t font_O[7] = {
	    0b01110,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b01110,
	    0b00000
	};

	uint8_t font_P[7] = {
	    0b11110,
	    0b10001,
	    0b11110,
	    0b10000,
	    0b10000,
	    0b10000,
	    0b00000
	};

	uint8_t font_Q[7] = {
	    0b01110,
	    0b10001,
	    0b10001,
	    0b10101,
	    0b10010,
	    0b01101,
	    0b00000
	};

	uint8_t font_R[7] = {
	    0b11110,
	    0b10001,
	    0b11110,
	    0b10100,
	    0b10010,
	    0b10001,
	    0b00000
	};

	uint8_t font_S[7] = {
	    0b01111,
	    0b10000,
	    0b01110,
	    0b00001,
	    0b00001,
	    0b11110,
	    0b00000
	};

	uint8_t font_T[7] = {
	    0b11111,
	    0b00100,
	    0b00100,
	    0b00100,
	    0b00100,
	    0b00100,
	    0b00000
	};

	uint8_t font_U[7] = {
	    0b10001,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b01110,
	    0b00000
	};

	uint8_t font_V[7] = {
	    0b10001,
	    0b10001,
	    0b10001,
	    0b10001,
	    0b01010,
	    0b00100,
	    0b00000
	};

	uint8_t font_W[7] = {
	    0b10001,
	    0b10001,
	    0b10101,
	    0b10101,
	    0b11011,
	    0b10001,
	    0b00000
	};

	uint8_t font_X[7] = {
	    0b10001,
	    0b01010,
	    0b00100,
	    0b00100,
	    0b01010,
	    0b10001,
	    0b00000
	};

	uint8_t font_Y[7] = {
	    0b10001,
	    0b10001,
	    0b01110,
	    0b00100,
	    0b00100,
	    0b00100,
	    0b00000
	};

	uint8_t font_Z[7] = {
	    0b11111,
	    0b00010,
	    0b00100,
	    0b01000,
	    0b10000,
	    0b11111,
	    0b00000
	};
	uint8_t font_block[7] = {
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b11111,
		0b00000

	};



//constant for all of the characters
	const uint8_t* font_data[54] = {font_a, font_b, font_c, font_d, font_e, font_f, font_g, font_h, font_i, font_j, font_k, font_l, font_m, font_n, font_o, font_p, font_q, font_r, font_s, font_t, font_u, font_v, font_w, font_x, font_y, font_z, font_A, font_B, font_C, font_D, font_E, font_F, font_G, font_H, font_I, font_J, font_K, font_L, font_M, font_N, font_O, font_P, font_Q, font_R, font_S, font_T, font_U, font_V, font_W, font_X, font_Y, font_Z, font_greater_than, font_block};

//more characters to be added as the game needs
	void LCD_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bgcolor, uint8_t scale) {
		if ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z') && c != '>' && c != '#') {
		   return; // Do nothing if character is outside 'A'-'Z', 'a'-'z', and '>' range
		}

	    // Find the font array for the character
	    const uint8_t* char_data;
	    if (c >= 'a' && c <= 'z') {
	        char_data = font_data[c - 'a']; // Get lowercase letter data
	    } else if (c >= 'A' && c <= 'Z') {
	        char_data = font_data[c - 'A' + 26]; // Use uppercase letters
	    } else if (c == '>') {
	        char_data = font_data[52]; // Get '>' character data
	    } else if (c == '#') {
	    	char_data = font_data[53];
	    }

	    // Draw each pixel as a larger block (scale x scale pixels)
	    for (int row = 0; row < 7; row++) {
	        uint8_t row_data = char_data[row]; // Get row pattern
	        for (int col = 0; col < 5; col++) {
	            if (row_data & (1 << (4 - col))) { // If pixel should be drawn
	                // Scale up by drawing multiple pixels
	                for (int i = 0; i < scale; i++) {
	                    for (int j = 0; j < scale; j++) {
	                        LCD_Drawpixel(x + col * scale + i, y + row * scale + j, color);
	                    }
	                }
	            }
	        }
	    }
	}


void LCD_Drawpixel(uint16_t x, uint16_t y, uint16_t color) {
	x = x + 35;
	// boundaries are 239 and 319 with x_offset of 35
    // Set the column address
    LCD_WriteCommand(0x2A); // Column address set command (ST7789)
    LCD_WriteData(x >> 8);  // Send the higher 8 bits of the x-coordinate
    LCD_WriteData(x & 0xFF); // Send the lower 8 bits of the x-coordinate

    // Set the row address
    LCD_WriteCommand(0x2B); // Row address set command (ST7789)
    LCD_WriteData(y >> 8);  // Send the higher 8 bits of the y-coordinate
    LCD_WriteData(y & 0xFF); // Send the lower 8 bits of the y-coordinate

    // Write the pixel color data
    LCD_WriteCommand(0x2C); // Memory write command (ST7789)
    LCD_WriteData(color >> 8);  // Send the higher 8 bits of the color
    LCD_WriteData(color & 0xFF); // Send the lower 8 bits of the color
}


// Draw a string on the display
void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgcolor, uint8_t scale) {
    while (*str) {
        LCD_DrawChar(x, y, *str, color, bgcolor, scale);
        x += (6 * scale); // Move x for the next character based on scale
        str++;
    }
}


// Draw a single pixel on the display
void LCD_SetPixel(uint16_t x, uint16_t y, uint32_t color) {
    LCD_WriteCommand(0x2A); // Column address set
    LCD_WriteData(x >> 8);
    LCD_WriteData(x & 0xFF);
    LCD_WriteCommand(0x2B); // Row address set
    LCD_WriteData(y >> 8);
    LCD_WriteData(y & 0xFF);
    LCD_WriteCommand(0x2C); // RAM write
    LCD_WriteData(color >> 16); // Red
    LCD_WriteData((color >> 8) & 0xFF); // Green
    LCD_WriteData(color & 0xFF); // Blue
}

// Draw a 10x10 wall (filled rectangle)
void DrawWall(uint16_t x, uint16_t y, uint16_t color) {
    // Draw a filled 10x10 square starting at (x,y)
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            LCD_Drawpixel(x + j, y + i, color);
        }
    }
}

// Draw a snake body segment as a filled circle
void DrawSnakeBody(uint16_t cx, uint16_t cy, uint16_t color) {
    // Use a radius of 5 pixels (approximately 10x10 overall)
    int radius = 5;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            // If the point is within the circle, draw the pixel
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                LCD_Drawpixel(cx + dx, cy + dy, color);
            }
        }
    }
}

// Draw an apple as a filled circle (you could also add an outline or extra detail)
void DrawApple(uint16_t cx, uint16_t cy, uint16_t color) {
    // Use a radius of 5 pixels for the apple as well
    int radius = 5;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                LCD_Drawpixel(cx + dx, cy + dy, color);
            }
        }
    }
}



