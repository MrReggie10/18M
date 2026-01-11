/*
 * servo.c
 *
 *  Created on: Jan 11, 2026
 *      Author: jacobyakubisin
 */

#include "stdio.h"
#include "servo.h"

servo_t servo_new(uint32_t *ccr)
{
  assert(ccr != NULL);

  servo *s = ccr;

  return s;
}

void servo_angle(servo_t s, uint32_t angle)
{
  assert(s != NULL);
  assert(0 <= angle && angle <= 180);

  *s = 1000 + ((1000*angle)/180);
}
