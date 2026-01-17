/*
 * claude_lcd.c
 *
 *  Created on: Jan 16, 2026
 *      Author: jacobyakubisin
 */

#include <font.h>
#include "font.h"
#include "claude_lcd.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

static SPI_HandleTypeDef *lcd_spi;  // Private pointer to SPI handle

// Create custom color:
uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void ST7796S_SetSPI(SPI_HandleTypeDef *hspi) {
    lcd_spi = hspi;
}

void ST7796S_Init(void) {
    // Hardware reset
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_Delay(120);

    // Software reset
    ST7796S_WriteCommand(0x01);  // SWRESET
    HAL_Delay(120);

    // Sleep Out
    ST7796S_WriteCommand(0x11);  // SLPOUT
    HAL_Delay(120);

    // Pixel Format Set - 16bit/pixel (RGB565)
    ST7796S_WriteCommand(0x3A);  // COLMOD
    ST7796S_WriteData(0x55);     // 16-bit color

    // Memory Access Control (orientation and color order)
    ST7796S_WriteCommand(0x36);  // MADCTL
    ST7796S_WriteData(0x48);     // Try different values: 0x48, 0x28, 0x88, 0xE8
                                 // Bit 3: BGR/RGB order
                                 // Bits 5,6,7: rotation/mirror

    // Display Inversion Off (try ON if colors look inverted)
    ST7796S_WriteCommand(0x20);  // INVOFF (use 0x21 for INVON)

    // Normal Display Mode
    ST7796S_WriteCommand(0x13);  // NORON
    HAL_Delay(10);

    // Display ON
    ST7796S_WriteCommand(0x29);  // DISPON
    HAL_Delay(120);
}

//void ST7796S_Draw

void ST7796S_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column Address Set
    ST7796S_WriteCommand(0x2A);  // CASET
    ST7796S_WriteData16(x0);     // Start column
    ST7796S_WriteData16(x1);     // End column

    // Row Address Set
    ST7796S_WriteCommand(0x2B);  // RASET
    ST7796S_WriteData16(y0);     // Start row
    ST7796S_WriteData16(y1);     // End row

    // Memory Write
    ST7796S_WriteCommand(0x2C);  // RAMWR - ready to send pixel data
}

void ST7796S_FillScreen(uint16_t color) {
    ST7796S_SetWindow(0, 0, 319, 479);  // Full screen (480x320)

    // Now send color data for every pixel
    for(uint32_t i = 0; i < 480 * 320; i++) {
        ST7796S_WriteData16(color);
    }
}

void ST7796S_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // Set window to rectangle bounds
    ST7796S_SetWindow(x, y, x + w - 1, y + h - 1);

    // Fill with color
    for(uint32_t i = 0; i < w * h; i++) {
        ST7796S_WriteData16(color);
    }
}


void ST7796S_DrawChar(uint16_t x, uint16_t y, char c, const Font *font,
                      uint16_t color, uint16_t bg_color) {

    // Check if character is in range
    if(c < font->first_char || c > font->last_char) {
        c = '?';  // Replace out-of-range chars with '?'
    }

    // Calculate offset in font data
    uint16_t char_offset = (c - font->first_char) * (font->height * 3);

    // Set drawing window for the character
    ST7796S_SetWindow(x, y, x + font->width - 1, y + font->height - 1);

    // Draw each row of the character
    for(uint8_t row = 0; row < font->height; row++) {

        // Draw each pixel in the row
        for(uint8_t col = 0; col < font->width; col++) {
            uint8_t line = font->data[char_offset + (row * 3) + (col / 8)];
            // Check if bit is set (starting from MSB)
            if(line & (1 << (8 - 1 - (col % 8)))) {
                ST7796S_WriteData16(color);  // Draw foreground
            } else {
                ST7796S_WriteData16(bg_color);  // Draw background
            }
        }
    }
}

void ST7796S_DrawString(uint16_t x, uint16_t y, const char *str, const Font *font,
                        uint16_t color, uint16_t bg_color) {

    uint16_t cursor_x = x;
    uint16_t cursor_y = y;

    while(*str) {
        if(*str == '\n') {
            // New line
            cursor_x = x;
            cursor_y += font->height + 2;  // 2 pixel spacing between lines
        } else if(*str == '\r') {
            // Carriage return
            cursor_x = x;
        } else {
            // Draw character
            ST7796S_DrawChar(cursor_x, cursor_y, *str, font, color, bg_color);
            cursor_x += font->width;  // Move cursor to next position
        }
        str++;
    }
}

void ST7796S_DrawInt(uint16_t x, uint16_t y, int32_t num, const Font *font,
                     uint16_t color, uint16_t bg_color) {

    char buffer[12];  // Enough for 32-bit int + sign + null
    sprintf(buffer, "%d", (int)num);
    ST7796S_DrawString(x, y, buffer, font, color, bg_color);
}

void ST7796S_DrawFloat(uint16_t x, uint16_t y, float num, uint8_t decimals,
                       const Font *font, uint16_t color, uint16_t bg_color) {
    char buffer[32];
    char format[8];
    sprintf(format, "%%.%df", decimals);
    sprintf(buffer, format, num);
    ST7796S_DrawString(x, y, buffer, font, color, bg_color);
}

uint16_t ST7796S_GetStringWidth(const char *str, const Font *font) {
    return strlen(str) * font->width;
}


void ST7796S_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  // DC = 0 for command
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);  // CS = 0 (select)
    HAL_SPI_Transmit(lcd_spi, &cmd, 1, 100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);    // CS = 1 (deselect)
}

void ST7796S_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);    // DC = 1 for data
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);  // CS = 0
    HAL_SPI_Transmit(lcd_spi, &data, 1, 100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);    // CS = 1
}

void ST7796S_WriteData16(uint16_t data) {
    ST7796S_WriteData(data >> 8);    // High byte
    ST7796S_WriteData(data & 0xFF);  // Low byte
}
