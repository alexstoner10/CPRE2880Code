/*
 * movement.c
 *
 *  Created on: Sep 10, 2025
 *      Author: astoner
 */

#include "movement.h"
#include "open_interface.h"
#include "lcd.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>

void move_forward(oi_t *sensor, int centimeters)
{

    double sum = 0;
    int speedLeft = 100;
    int speedRight = 100;
    oi_setWheels(speedRight, speedLeft);
    int distance = centimeters * 10;
    while (sum < distance - 100)
    {
        if (sensor->bumpLeft)
        {
            int leftSum = 0;
            left_bumper(sensor);
            while (leftSum < 130)
            {
                oi_setWheels(speedRight, speedLeft);

                oi_update(sensor);
                leftSum += sensor->distance;
            }
            oi_setWheels(0, 0);
            turn_clockwise(sensor, 83);
            oi_setWheels(0, 0);
            sensor->bumpLeft = 0;
            break;

        }
        else if (sensor->bumpRight)
        {
            int rightSum = 0;
            right_bumper(sensor);
            while (rightSum < 130)
            {
                oi_setWheels(speedRight, speedLeft);
                oi_update(sensor);
                rightSum += sensor->distance;
            }
            oi_setWheels(0, 0);
            turn_clockwise(sensor, -83);
            oi_setWheels(0, 0);
            sensor->bumpRight = 0;
            break;
        }
        if (speedRight <= 500 && speedLeft <= 500)
        {
            speedRight++;
            speedLeft++;
        }

        oi_setWheels(speedRight, speedLeft);
        oi_update(sensor);
        sum += sensor->distance;
    }
    oi_setWheels(0, 0);

}
void turn_clockwise(oi_t *sensor, int degrees)
{
    int speedLeft = 0;
    int speedRight = 0;
    if (degrees > 0)
    {
        speedRight = 100;
        speedLeft = -100;
    }
    else
    {
        speedLeft = 100;
        speedRight = -100;
    }
    oi_setWheels(speedRight, speedLeft);
    double degreesTurned = 0;

    if (degrees > 0)
    {
        while (degreesTurned < degrees)
        {
            oi_update(sensor);
            degreesTurned += sensor->angle;
        }
    }
    else
    {
        while (degreesTurned > degrees)
        {
            oi_update(sensor);
            degreesTurned += sensor->angle;
        }
    }

    oi_setWheels(0, 0);

}
void move_backward(oi_t *sensor, int centimeters)
{

    int speedLeft = -100;
    int speedRight = -100;
    int distance = centimeters * 10;
    double sum = distance - 40;
    while (sum > 0)
    {

        if (speedRight >= -500 && speedLeft >= -500)
        {
            speedRight--;
            speedLeft--;
        }

        oi_setWheels(speedRight, speedLeft);
        oi_update(sensor);
        sum += sensor->distance;
    }
    oi_setWheels(0, 0);
}
void left_bumper(oi_t *sensor)
{
    sendingString("\r\nObject hit on left side");
    oi_setWheels(0, 0);
    move_backward(sensor, 15);
    oi_setWheels(0, 0);
    turn_clockwise(sensor, -83);
    oi_setWheels(0, 0);
}
void right_bumper(oi_t *sensor)
{
    sendingString("\r\nObject hit on right side");
    oi_setWheels(0, 0);
    move_backward(sensor, 15);
    oi_setWheels(0, 0);
    turn_clockwise(sensor, 83);
    oi_setWheels(0, 0);
}

/*
 *
 *void move_forward(oi_t *sensor, int centimeters){

 double sum = 0;
 int speedLeft = 100;
 int speedRight= 100;
 oi_setWheels(speedRight, speedLeft);
 int distance = centimeters * 10;
 while (sum < distance - 100){
 if(sensor->bumpLeft){
 int leftSum = 0;
 left_bumper(sensor);
 while (leftSum < 250){
 oi_setWheels(speedRight, speedLeft);

 oi_update(sensor);
 leftSum += sensor->distance;
 }
 oi_setWheels(0, 0);
 turn_clockwise(sensor, 83);
 oi_setWheels(0, 0);
 sum -= 150;
 sensor->bumpLeft = 0;
 }else if(sensor->bumpRight){
 int rightSum = 0;
 right_bumper(sensor);
 while (rightSum < 250){
 oi_setWheels(speedRight, speedLeft);
 oi_update(sensor);
 rightSum += sensor->distance;
 }
 oi_setWheels(0, 0);
 turn_clockwise(sensor, -83);
 oi_setWheels(0, 0);
 sum -= 150;
 sensor->bumpRight = 0;

 }
 if(speedRight <= 500 && speedLeft <= 500){
 speedRight++;
 speedLeft++;
 }

 oi_setWheels(speedRight, speedLeft);
 oi_update(sensor);
 sum += sensor->distance;
 }
 oi_setWheels(0, 0);


 }
 */

