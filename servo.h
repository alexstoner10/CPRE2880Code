/*
 * servo.h
 *
 *  Created on: Nov 5, 2025
 *      Author: astoner
 */

#ifndef SERVO_H_
#define SERVO_H_
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include <stdint.h>


void servo_init(void);
int servo_move(float degrees);
void servo_cal(void);
uint32_t match_value(float degrees);



#endif /* SERVO_H_ */
