/*
 * adc.c
 *
 *  Created on: Oct 22, 2025
 *      Author: astoner
 */

#include "adc.h"

void adc_init(void){
    SYSCTL_RCGCGPIO_R |= 0b000010;
    SYSCTL_RCGCADC_R |= 0b000001;
	
	
    GPIO_PORTB_AFSEL_R |= 0b00010000;
    GPIO_PORTB_DEN_R &= 0b11101111;
    GPIO_PORTB_AMSEL_R |= 0b00010000;
    GPIO_PORTB_DIR_R &= 0b11101111;

    ADC0_ACTSS_R &= 0b1101;
    ADC0_EMUX_R &= 0x0F;
    ADC0_SSMUX1_R = 0x000A;
    ADC0_SSCTL1_R |= 0x0006;
    ADC0_SAC_R = 0x4;
    ADC0_ACTSS_R |= 0b0010;



}
int adc_read(void){

    ADC0_PSSI_R |= 0b0010;
    int data;
    while((ADC0_RIS_R & 0x2) == 0){

    }
    data = ADC0_SSFIFO1_R & 0xFFF;
	ADC0_ISC_R = 0x02;
    return data;
}



