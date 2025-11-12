/*
 * movement.h
 *
 *  Created on: Sep 10, 2025
 *      Author: astoner
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
#include "open_interface.h"
void move_forward(oi_t *sensor, int centimeters);
void turn_clockwise(oi_t *sensor, int degrees);
void left_bumper(oi_t *sensor);
    void right_bumper(oi_t *sensor);



#endif /* MOVEMENT_H_ */
