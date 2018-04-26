#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"
#include "Switch.h"
#include "NFC.h"
#include "Coin.h"
#include "LCD.h"
#include "SysTick.h"
#include "ST7735.h"

#define F16HZ (50000000/16)
#define F20KHZ (50000000/20000)

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

extern char response[];
extern int response_pointer;

int sessionAmount = 0;
//state 0: begin transaction, state 1: transaction in progress, state 2: "confirm" 
//pressed, state 3: transaction finished successfully
int currentState = 0;

void stateChanged() {
	drawScreen(currentState);						
}

void Button_Handler(int button){
	// button 0: confirm, 1: cancel
	switch(button) {
		case 0: {
				if (currentState == 1) {
					currentState = 2;
				}
				else if (currentState == 2) {
					currentState = 3;
					writeValue(sessionAmount);
				}
				
				stateChanged();
		} break;
		case 1: {
				if (currentState == 1) {
					// print some sort of "Want to cancel? Your money will be lost." statement
				}
				else if (currentState == 2) {
					currentState = 1;
					stateChanged();
				}
		} break;
		default: {
				//
		}
	}
}

void Coin_Handler(void){
	sessionAmount+=5; // nickel, dime, quarter
	drawScreen(currentState); //TO-DO: just update amount in future
}

void initPortE() {
	// initialize green LED
	SYSCTL_RCGCGPIO_R |= 0x10;            // activate port E
	GPIO_PORTE_DIR_R |= 0x20;
	GPIO_PORTE_DEN_R |= 0x20;
}

void initPortF() {
	SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
	
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
}

void initNFC() {
	SysTick_Init(80000, finishRelease);
	setupNFCBoard();
}

int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
	DisableInterrupts();
	ST7735_InitR(INITR_REDTAB);
	initPortF();
	initPortE();
	Buttons_Init(Button_Handler);
	setupCoinSelector(Coin_Handler);
	EnableInterrupts();
	
	GPIO_PORTF_DATA_R ^= 0x04;  
	//writeValue(876);
	while(readTag() == 0);	
	GPIO_PORTF_DATA_R ^= 0x04;  
	readTag();
	GPIO_PORTF_DATA_R ^= 0x04;  
	
	// initialize green LED
	SYSCTL_RCGCGPIO_R |= 0x10;            // activate port E
	GPIO_PORTE_DIR_R |= 0x20;
	GPIO_PORTE_DEN_R |= 0x20;
	
	drawScreen(currentState);

	while(1) {
		//state 0
		//setup periodic timer to read tag
		
		while (currentState == 0) {
			if (isTagConnected() == 1) {
				currentState = 1;
				stateChanged();
			}
		}
		
		//on receive connected message -> transition state (turn off NFC RF)
		//state 1
		//wait for input from Coin Selector -> update screen
		//wait for button press from Switch -> transition state (write to tag and turn on NFC RF)
		//state 2
		//on receive completed message -> transition state
	}
}
