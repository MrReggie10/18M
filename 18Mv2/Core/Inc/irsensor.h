/*
 * irsensor.h
 *
 *  Created on: Jan 13, 2026
 *      Author: jacobyakubisin
 */

#ifndef INC_IRSENSOR_H_
#define INC_IRSENSOR_H_

#include "main.h"
#include "stdio.h"
#include <stdint.h>

GPIO_PinState ir_read_penny();

GPIO_PinState ir_read_nickel();

GPIO_PinState ir_read_dime();

GPIO_PinState ir_read_quarter();

#endif /* INC_IRSENSOR_H_ */
