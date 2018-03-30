//Timer0A.c
//Controls the note changes based on note duration

#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"
#include "Music.h"
#include "Switch.h"
#include "Systick.h"

extern Music cMusic;
int pointer = 0, noteCounter = 0;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

//set period to shortest note duration
void Timer0A_Init(uint32_t period){
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
}

void rewindMusic() {
	noteCounter = 0;
	pointer = 0;
}

void stopMusic() {
	  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
}

void startMusic() {
	  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
}

void Timer0A_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x02;       // toggle PF2
	GPIO_PORTF_DATA_R ^= 0x02;       // toggle PF2

	/*pointer++;
	if(cMusic.notes[noteCounter].length <= pointer) {
		noteCounter++;
		pointer = 0;
		// break inbetween notes
	  SysTick_Disable();
		Sound_FChange(cMusic.notes[noteCounter].pitch);
		//for (int i = 0; i < 1000; i ++)
		SysTick_Enable();
	}
	
	if(noteCounter >= cMusic.length) {
		stopMusic();
	}*/
	
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	GPIO_PORTF_DATA_R ^= 0x02;
}
