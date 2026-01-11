/*
 * servo.h
 *
 *  Created on: Jan 11, 2026
 *      Author: jacobyakubisin
 */

#include "stdio.h"
#include "main.h"

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

typedef struct uint32_t *servo;
typedef servo* servo_t;

/*
@requires ccr != NULL
*/
servo_t servo_new(uint32_t *ccr);

/*
@requires s != NULL
@requires 0 <= angle && angle <= 180
*/
void servo_angle(servo_t s, uint32_t angle);

#endif /* INC_SERVO_H_ */
