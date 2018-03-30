// Button.c
// This software configures the off-board piano keys
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 8/25/2014 
// Last Modified: 3/6/2015 
// Section 1-2pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timer1.h"
#define NVIC_EN0_INT4           0x00000010  // Interrupt 4 enable

void (*InputTask)(int i);   // user function

void Buttons_Arm(void) {
	GPIO_PORTE_ICR_R = 0x07;    // clear flag6-4
  GPIO_PORTE_IM_R |= 0x07;    // enable interrupt on PE6-4
                              // GPIO PortE=priority 2
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000040; // bits 5-7
  NVIC_EN0_R = NVIC_EN0_INT4; // enable interrupt 4 in NVIC
}

void Buttons_Init(void(*task)(int i)) {
  InputTask = task;          // user function	unsigned long volatile delay;
	SYSCTL_RCGCGPIO_R |= 0x10;
	GPIO_PORTE_DIR_R &= ~0x07;
	GPIO_PORTE_DEN_R |= 0x07; 
  GPIO_PORTE_IS_R &= ~0x07;   // PE6-4 is edge-sensitive 
  GPIO_PORTE_IBE_R &= ~0x07;  // PE6-4 is not both edges 
  GPIO_PORTE_IEV_R |= 0x07;   // PE6-4 rising edge event
	Buttons_Arm();
}

void GPIOPortE_Handler(void){
	GPIO_PORTE_IM_R &= ~0x07;     // disarm interrupt 
	int rVal = -1;
	
  if(GPIO_PORTE_RIS_R&0x01){  // poll PE0
    GPIO_PORTE_ICR_R = 0x01;  // acknowledge flag0
		rVal = 0;
  }
  if(GPIO_PORTE_RIS_R&0x02){  // poll PE1
    GPIO_PORTE_ICR_R = 0x02;  // acknowledge flag1
		rVal = 1;
  }
	if(GPIO_PORTE_RIS_R&0x04){  // poll PE2
    GPIO_PORTE_ICR_R = 0x04;  // acknowledge flag2
		rVal = 2;
  }
	
	(*InputTask)(rVal);
	Timer1_Init();
}
