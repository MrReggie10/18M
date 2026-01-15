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

uint8_t ir_read_penny();

uint8_t ir_read_nickel();

uint8_t ir_read_dime();

uint8_t ir_read_quarter();

#endif /* INC_IRSENSOR_H_ */
