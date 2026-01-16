/*
 * claude_lcd.h
 *
 *  Created on: Jan 16, 2026
 *      Author: jacobyakubisin
 */

#ifndef INC_CLAUDE_LCD_H_
#define INC_CLAUDE_LCD_H_

#include "stm32f4xx_hal.h"

// Common colors in RGB565:
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F

uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b);
void ST7796S_SetSPI(SPI_HandleTypeDef *hspi);
void ST7796S_Init(void);
void ST7796S_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7796S_FillScreen(uint16_t color);
void ST7796S_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7796S_WriteCommand(uint8_t cmd);
void ST7796S_WriteData(uint8_t data);
void ST7796S_WriteData16(uint16_t data);

#endif /* INC_CLAUDE_LCD_H_ */
