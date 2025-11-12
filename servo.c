/*
 * servo.c
 *
 *  Created on: Nov 5, 2025
 *      Author: astoner
 */



#include "servo.h"
#include <stdint.h>
#include "lcd.h"


//for cybot 26 downstairs, 48670 in MATCHR is 0 degrees   20500 is 180 degrees so 34585 is 90 degrees
// cybot 1 upstairs, 50600 and 22750
//cybot 6 upstairs, 49850 and 22750

// to calibrate roughly, keep changing MATCHR in servo_init() until you find approximate 0 and 180 degrees
// eventually add functionality to use buttons to do this so it is much faster to calibrate
uint32_t match;

uint32_t match_value(float degrees);

void servo_init(void){
    SYSCTL_RCGCGPIO_R |= 0b000010;
    SYSCTL_RCGCTIMER_R |= 0b0010;
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0b100000;
    GPIO_PORTB_AFSEL_R &= ~0x20;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R &= 0xFF0FFFFF;
    GPIO_PORTB_PCTL_R |= 0x700000;

    TIMER1_CTL_R &= 0xFEFF;

    TIMER1_CFG_R = 0x04;

    TIMER1_TBMR_R |= 0x0A;
    //TIMER1_TBMR_R &= ~0x10;

    TIMER1_TBILR_R = 57856;    //E200
    TIMER1_TBPR_R = 4;   //0x40000
    TIMER1_TBMATCHR_R = 36300;   //41856; //304,000 ticks: 320,000-16000 (1/20 of 320000) = 304,000
    TIMER1_TBPMR_R = 4;
    TIMER1_CTL_R |= 0x100;

}
int servo_move(float degrees){
    if (degrees < 0) degrees = 0;
    if (degrees > 180) degrees = 180;

    float pulse_width = 49850 - (degrees /180.0) * (49850 - 22750);
    match = (uint32_t)(pulse_width);

    TIMER1_TBMATCHR_R = match & 0xFFFF;
    //timer_waitMillis(1000);
    //TIMER1_TBPMR_R = (match << 16) & 0xFF; //might need to remove, i think it should stay 4 the whole time regardless what angle is put in
    return 0;
}

uint32_t match_value(float degrees){
    if (degrees < 0) degrees = 0;
    if (degrees > 180) degrees = 180;

    float pulse_width1 = 48670 - (degrees /180.0) * (48670 - 20500);
    uint32_t match1 = (uint32_t)(pulse_width1);

    return (uint32_t)(match1);

}


