#ifndef SERVO_H
#define SERVO_H

typedef struct uint32_t *servo;
typedef servo* servo_t;

servo_t servo_new(uint32_t *ccr);

void servo_angle(servo_t s, uint_32 angle);

#endif
