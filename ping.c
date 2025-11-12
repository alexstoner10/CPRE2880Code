/*
 * ping.c
 *
 *  Created on: Oct 29, 2025
 *      Author: astoner
 */

#include "ping.h"

volatile enum
{
    LOW, HIGH, DONE
} state = LOW;
volatile unsigned long rising_time; //Pulse start time: Set by ISR
volatile unsigned long falling_time; //Pulse end time: Set by ISR
volatile int count = 0;
int overflow = 0;
void ping_init(void)
{
    SYSCTL_RCGCGPIO_R |= 0b000010;
    SYSCTL_RCGCTIMER_R |= 0b01000;
    GPIO_PORTB_DEN_R |= 0x8;

    TIMER3_CTL_R &= 0xFEFF;
    TIMER3_CFG_R = 0x04;

    TIMER3_TBMR_R |= 0x07;
    TIMER3_TBMR_R &= ~0x10;

    TIMER3_CTL_R |= 0x0C00;
    TIMER3_TBILR_R = 0xFFFF;
    TIMER3_TBPR_R = 0xFF;
    TIMER3_ICR_R |= 0x400;
    TIMER3_IMR_R |= 0x400;
    NVIC_EN1_R |= 0b10000;
    TIMER3_CTL_R |= 0x0100;

    IntRegister(INT_TIMER3B, Timer3B_Handler);
    IntMasterEnable();

}

float ping_read() // Get distance from PING sensor
{
    float pulse_width = 0;


    send_pulse(); // Send short pulse to request PING burst
// Wait for ISR to capture rising edge & falling edge time

    while (state != DONE);
    state = LOW;

    int overflow = (falling_time > rising_time);

    if (rising_time > falling_time)
    {
        pulse_width = rising_time - falling_time;
    }
    else
    {
        //lcd_printf("Overflow");
        pulse_width = ((unsigned long) overflow<<24) + rising_time - falling_time;
        count++;
    }

    // Calculate the width of the pulse; convert to centimeters
    float time_sec = pulse_width / 16000000.0;
    //float time_ms = time_sec * 1000;

     float distance = (time_sec / 2) * 343 * 100;
     distance -= 5;
     //lcd_printf("Clk: %.1f T: %.1f D: %.1f C: %d", pulse_width, time_ms, distance, count);
     return distance;
}
void send_pulse()
{
    TIMER3_CTL_R &= 0xFEFF;
    TIMER3_IMR_R &= ~0x400;
    GPIO_PORTB_AFSEL_R &= 0x7;
    TIMER3_IMR_R &= 0xFBFF;
    GPIO_PORTB_DIR_R |= 0b1000; // Set PB3 as output
	GPIO_PORTB_DATA_R &= 0xF7;
	timer_waitMicros(5);
    GPIO_PORTB_DATA_R |= 0x08; // Set PB3 to high
// wait at least 5 microseconds based on data sheet
	timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= 0xF7; // Set PB3 to low
    GPIO_PORTB_DIR_R &= 0b0111; // Set PB3 as input
    TIMER3_ICR_R |= 0x400;


    GPIO_PORTB_AFSEL_R |= 0x08;
    GPIO_PORTB_PCTL_R &= 0xFFFF0FFF;
    GPIO_PORTB_PCTL_R |= 0x7000;


    TIMER3_IMR_R |= 0x400;
    TIMER3_CTL_R |= 0x0100;

}
void Timer3B_Handler(void)
{

    if (TIMER3_MIS_R & 0x400)
    {
        if (state == LOW)
        {
            rising_time = (TIMER3_TBPS_R << 16) | TIMER3_TBR_R;
            state = HIGH;
        }
        else if (state == HIGH)
        {
            falling_time = (TIMER3_TBPS_R << 16) | TIMER3_TBR_R;
            state = DONE;
        }
        TIMER3_ICR_R |= 0x400;
    }
}

