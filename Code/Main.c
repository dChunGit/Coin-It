#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"
#include "Switch.h"
#include "NFC.h"
#include "Coin.h"
#include "LCD.h"

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

int sessionAmount = 0;
//state 0: begin transaction, state 1: transaction in progress, state 2: "confirm" pressed, state 3: transaction finished successfully
int currentState = 0;

void Button_Handler(int button){
	// button 0: confirm, 1: cancel
	switch(button) {
		case 0: {
						if (currentState == 1) {
							currentState = 2;
						}
						else if (currentState == 2) {
							currentState = 3;
						}
							drawScreen(currentState);						
				} break;
		case 1: {
						if (currentState == 2) {
							currentState = 1;
							drawScreen(currentState);
						}
				} break;
				default: {
						//
				}
	}
}

void Coin_Handler(){
	
}

int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
	DisableInterrupts();

	Buttons_Init(Button_Handler);
	setupCoinSelector(Coin_Handler);
	setupNFCBoard();
	
	// initialize green LED
	SYSCTL_RCGCGPIO_R |= 0x10;            // activate port E
	GPIO_PORTE_DIR_R |= 0x20;
	GPIO_PORTE_DEN_R |= 0x20;

	
	EnableInterrupts();
	
	
	
	while(1) {
		//wait for input from Coin Selector
		//wait for button press from Switch
		//update screen accordingly
	}

}
