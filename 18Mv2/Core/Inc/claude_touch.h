/*
 * claude_touch.h
 *
 *  Created on: Jan 16, 2026
 *      Author: jacobyakubisin
 */

#ifndef INC_CLAUDE_TOUCH_H_
#define INC_CLAUDE_TOUCH_H_

#include "stm32f4xx_hal.h"  // Adjust for your STM32 family

#define XPT2046_CMD_X_READ  0xD0  // 11010000 - Read X, 12-bit, differential
#define XPT2046_CMD_Y_READ  0x90  // 10010000 - Read Y, 12-bit, differential
#define XPT2046_CMD_Z1_READ 0xB0  // 10110000 - Read Z1 (pressure)
#define XPT2046_CMD_Z2_READ 0xC0  // 11000000 - Read Z2 (pressure)

void XPT2046_SetSPI(SPI_HandleTypeDef *hspi);
void XPT2046_SetUART(UART_HandleTypeDef *huart);
void XPT2046_Init(void);
typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t touched;
} TouchPoint;
#define TOUCH_SAMPLES 10
TouchPoint XPT2046_GetTouchAvg(void);
typedef struct {
    int16_t x_min;
    int16_t x_max;
    int16_t y_min;
    int16_t y_max;
} TouchCalibration;
void XPT2046_GetScreenCoordinates(TouchPoint *tp, uint16_t *screen_x, uint16_t *screen_y);
void XPT2046_Calibrate(void);
void XPT2046_WriteRead(uint8_t cmd, uint8_t *data, uint16_t size);
uint16_t XPT2046_ReadData(uint8_t cmd);

#endif /* INC_CLAUDE_TOUCH_H_ */
