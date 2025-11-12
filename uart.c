/*
 *
 *   uart_extra_help.c
 * Description: This is file is meant for those that would like a little
 *              extra help with formatting their code, and followig the Datasheet.
 */

#include "uart.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>

#define REPLACE_ME 0x00

void uart_init(int baud)
{
    SYSCTL_RCGCGPIO_R |= 0b000010;      // enable clock GPIOB (page 340)
    SYSCTL_RCGCUART_R |= 0b00000010;  //0x02    // enable clock UART1 (page 344)
    GPIO_PORTB_AFSEL_R |= 0b00000011; //0x03 // sets PB0 and PB1 as peripherals (page 671)
    //GPIO_PORTB_PCTL_R &= 0x11111100;
    GPIO_PORTB_PCTL_R |= 0x00000011; //0x03 // pmc0 and pmc1       (page 688)  also refer to page 650
    GPIO_PORTB_DEN_R |= 0b00000011;       //0x03 // enables pb0 and pb1
    //GPIO_PORTB_DIR_R &= 0b11111100;
    GPIO_PORTB_DIR_R |= 0b00000010;    //0x01 // sets pb0 as output, pb1 as input

    //compute baud values [UART clock= 16 MHz] 
    double fbrd;
    int ibrd;

    fbrd = 44; //sff page 903
    ibrd = 8;

    UART1_CTL_R &= 0b10;      // disable UART1 (page 918)
    UART1_IBRD_R = ibrd;        // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;   // write fractional portion of BRD to FBRD
    UART1_LCRH_R = 0b01100000; //0x60    // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R = 0x0;          // use system clock as clock source (page 939)
    UART1_CTL_R |= (1 << 0) | (1 << 8) | (1 << 9);        // enable UART1
}

void uart_sendChar(char data)
{

    while (UART1_FR_R & 0x20)
    {
    }
    UART1_DR_R = data;

}

char uart_receive(void)
{

    char data;
    if (UART1_FR_R & 0x10)
    {
        return '0';
    }
    unsigned int received = UART1_DR_R;

    if (received & 0xF00)
    {
        return '0';
    }
    else
    {
        data = (char) received & 0xFF;
    }
    return data;
}

void uart_sendStr(const char *data)
{
    //TODO

}

// _PART3

void uart_interrupt_init()
{
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0x0010; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0x00000040; //enable uart1 interrupts - page 104

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number

}

void uart_interrupt_handler()
{
    char data;
    unsigned int received;
// STEP1: Check the Masked Interrup Status
    if (UART1_MIS_R & 0x10)
    {
        received = UART1_DR_R;
       data = (char) received & 0xFF;

//STEP2:  Copy the data 

    if (data == '\r')
    {
        uart_sendChar(data);
        uart_sendChar('\n');
    }
    else
    {
        uart_sendChar(data);
    }
//STEP3:  Clear the interrup   
    UART1_ICR_R |= 0x10;
    }
}
void sendingString(char *sendString)
{
    int i;
    for (i = 0; i < strlen(sendString); i++)
    {

        uart_sendChar(sendString[i]);
    }
}

char uart_receive_blocking(void)
{

    char data;
        while (UART1_FR_R & 0x10)
        {

        }
        unsigned int received = UART1_DR_R;

        if (received & 0xF00)
        {

        }
        else
        {
            data = (char) received & 0xFF;
        }
        return data;
}
void uart_flush_input(void){
    volatile char dummy;
    while(!(UART1_FR_R & 0x10)){
        dummy = UART1_DR_R;
    }
}
