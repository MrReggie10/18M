/*
 * claude_touch.c
 *
 *  Created on: Jan 16, 2026
 *      Author: jacobyakubisin
 */

#include "claude_touch.h"
#include "stm32f4xx_hal.h"  // Adjust for your STM32 family
#include "claude_lcd.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

static SPI_HandleTypeDef *touch_spi;  // Private pointer to SPI handle
static UART_HandleTypeDef *console_uart;  // Private pointer to SPI handle

// Pin definitions - adjust to your setup
#define TOUCH_CS_PIN     GPIO_PIN_6
#define TOUCH_CS_PORT    GPIOC
#define TOUCH_IRQ_PIN    GPIO_PIN_8   // Optional: touch interrupt pin
#define TOUCH_IRQ_PORT   GPIOC

void XPT2046_SetSPI(SPI_HandleTypeDef *hspi) {
	touch_spi = hspi;
}

void XPT2046_SetUART(UART_HandleTypeDef *huart) {
	console_uart = huart;
}

void XPT2046_Init(void) {
    // Set CS high (idle state)
    HAL_GPIO_WritePin(TOUCH_CS_PORT, TOUCH_CS_PIN, GPIO_PIN_SET);

    // Small delay to stabilize
    HAL_Delay(10);

    // Optional: Read a dummy value to initialize
    XPT2046_ReadData(XPT2046_CMD_X_READ);
}

#define TOUCH_SAMPLES 10

TouchPoint XPT2046_GetTouchAvg(void) {
    TouchPoint tp = {0};
    uint32_t x_sum = 0;
    uint32_t y_sum = 0;
    uint8_t valid_samples = 0;

    // Check if touched
    if(HAL_GPIO_ReadPin(TOUCH_IRQ_PORT, TOUCH_IRQ_PIN) == GPIO_PIN_RESET) {
        // Take multiple samples
        for(uint8_t i = 0; i < TOUCH_SAMPLES; i++) {
            uint16_t x = XPT2046_ReadData(XPT2046_CMD_X_READ);
            uint16_t y = XPT2046_ReadData(XPT2046_CMD_Y_READ);

            // Simple validation (ignore obviously bad readings)
            if(x > 100 && x < 3900 && y > 100 && y < 3900) {
                x_sum += x;
                y_sum += y;
                valid_samples++;
            }
        }

        if(valid_samples > 0) {
            tp.x = x_sum / valid_samples;
            tp.y = y_sum / valid_samples;
            tp.touched = 1;
        }
    }

    return tp;
}

TouchCalibration touch_cal = {
    .x_min = 0x207,   // Calibrate these values
    .x_max = 0xD95,
    .y_min = 0x1FE,
    .y_max = 0xD92
};

void XPT2046_GetScreenCoordinates(TouchPoint *tp, uint16_t *screen_x, uint16_t *screen_y) {
    if(!tp->touched) {
        *screen_x = 0;
        *screen_y = 0;
        return;
    }

    // Map touch coordinates to screen coordinates (480x320)
    *screen_x = (tp->x - touch_cal.x_min) * 480 / (touch_cal.x_max - touch_cal.x_min);
    *screen_y = (tp->y - touch_cal.y_min) * 320 / (touch_cal.y_max - touch_cal.y_min);

    // Clamp to screen bounds
    if(*screen_x > 479) *screen_x = 479;
    if(*screen_y > 319) *screen_y = 319;
}

void XPT2046_Calibrate(void) {
    // Draw crosshairs at corners and record touch values
    // Top-left corner
	ST7796S_FillRect(2, 2, 50, 50, RED);
    while(HAL_GPIO_ReadPin(TOUCH_IRQ_PORT, TOUCH_IRQ_PIN) != GPIO_PIN_RESET);
    TouchPoint tl = XPT2046_GetTouchAvg();
    HAL_Delay(500);

    // Bottom-right corner
    ST7796S_FillScreen(0x0000);
    ST7796S_FillRect(2, 2, 50, 50, GREEN);
    while(HAL_GPIO_ReadPin(TOUCH_IRQ_PORT, TOUCH_IRQ_PIN) != GPIO_PIN_RESET);
    TouchPoint br = XPT2046_GetTouchAvg();

    // Store calibration
    touch_cal.x_min = tl.x;
    touch_cal.y_min = tl.y;
    touch_cal.x_max = br.x;
    touch_cal.y_max = br.y;

    char buf[40];
    sprintf(buf, "xmin: 0x%02X\r\n", touch_cal.x_min);
    HAL_UART_Transmit(console_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    sprintf(buf, "ymin: 0x%02X\r\n", touch_cal.y_min);
    HAL_UART_Transmit(console_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    sprintf(buf, "xmax: 0x%02X\r\n", touch_cal.x_max);
    HAL_UART_Transmit(console_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
    sprintf(buf, "ymax: 0x%02X\r\n", touch_cal.y_max);
    HAL_UART_Transmit(console_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
}

void XPT2046_WriteRead(uint8_t cmd, uint8_t *data, uint16_t size) {
    HAL_GPIO_WritePin(TOUCH_CS_PORT, TOUCH_CS_PIN, GPIO_PIN_RESET);  // CS low
    HAL_SPI_Transmit(touch_spi, &cmd, 1, 100);
    HAL_SPI_Receive(touch_spi, data, size, 100);
    HAL_GPIO_WritePin(TOUCH_CS_PORT, TOUCH_CS_PIN, GPIO_PIN_SET);    // CS high
}

uint16_t XPT2046_ReadData(uint8_t cmd) {
    uint8_t data[2] = {0};

    HAL_GPIO_WritePin(TOUCH_CS_PORT, TOUCH_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(touch_spi, &cmd, 1, 100);
    HAL_SPI_Receive(touch_spi, data, 2, 100);
    HAL_GPIO_WritePin(TOUCH_CS_PORT, TOUCH_CS_PIN, GPIO_PIN_SET);

    // Combine bytes and shift (12-bit result in 16-bit container)
    return ((data[0] << 8) | data[1]) >> 3;  // Right shift 3 to get 12-bit value
}
