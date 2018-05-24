#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

int timeout_counter = 0;

void (*PeriodicTask)(void);   // user function


void SysTick_Init(uint32_t period, void(*task)(void)){
	
	PeriodicTask = task;
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period - 1; // reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2       
  // enable interrupts after all initialization is finished
}

void SysTick_Enable() {
	  NVIC_ST_CTRL_R = 0x07;
}

void SysTick_Disable() {
	  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
}

void SysTick_Handler() {
		if(timeout_counter < 40) {
			timeout_counter++;
		} else {
			SysTick_Disable();
			PeriodicTask();
		}
}
