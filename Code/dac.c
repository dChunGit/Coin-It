// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 8/25/2014 
// Last Modified: 3/6/2015 
// Section 1-2pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "SysTick.h"
#include "Music.h"

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	SYSCTL_RCGCSSI_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x01;
	while((SYSCTL_PRGPIO_R & 0x01) == 0){};
		
	GPIO_PORTA_AFSEL_R |= 0x2C;
	GPIO_PORTA_DEN_R |= 0x2C;
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFF0F00FF) + 0x00202200;
	GPIO_PORTA_AMSEL_R = 0;
		
	SSI0_CR1_R = 0x0;
	SSI0_CPSR_R = 0x02;
	SSI0_CR0_R &= ~(0x0000FFF0);
	SSI0_CR0_R |= 0x0F;
	SSI0_DR_R = 0;
	SSI0_CR1_R |= 0x02;
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint16_t data){
	while((SSI0_SR_R & 0x02) == 0){};
	SSI0_DR_R = data;
}

