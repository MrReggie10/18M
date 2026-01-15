/*
 * irsensor.c
 *
 *  Created on: Jan 13, 2026
 *      Author: jacobyakubisin
 */
#include "irsensor.h"
#include "stdio.h"
#include "main.h"

GPIO_PinState ir_read_penny()
{
  return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
}

GPIO_PinState ir_read_nickel()
{
  return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);
}

GPIO_PinState ir_read_dime()
{
  return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);
}

GPIO_PinState ir_read_quarter()
{
  return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);
}
