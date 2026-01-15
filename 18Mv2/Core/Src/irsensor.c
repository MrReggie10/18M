/*
 * irsensor.c
 *
 *  Created on: Jan 13, 2026
 *      Author: jacobyakubisin
 */
#include "irsensor.h"
#include "stdio.h"
#include "main.h"

uint8_t ir_read_penny()
{
  return (uint8_t) HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
}

uint8_t ir_read_nickel()
{
  return (uint8_t) HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);
}

uint8_t ir_read_dime()
{
  return (uint8_t) HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);
}

uint8_t ir_read_quarter()
{
  return (uint8_t) HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);
}
