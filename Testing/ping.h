/*
 * ping.h
 *
 *  Created on: Oct 29, 2025
 *      Author: astoner
 */

#ifndef PING_H_
#define PING_H_
#include <stdint.h>
#include "lcd.h"
#include "driverlib/interrupt.h"
#include "Timer.h"

void ping_init(void);
float ping_read();
void send_pulse();
void Timer3B_Handler(void);
uint32_t get_capture_value();
#endif /* PING_H_ */
