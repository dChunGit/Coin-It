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
//state 0: begin transaction, state 1: transaction in progress, state 2: "confirm" pressed, 
//state 3: transaction finished successfully
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
				else if(currentState == 3) {
					currentState = 0;
				}					
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

void Coin_Handler(void){
	sessionAmount += 5;
	drawScreen(currentState);
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
	//SysTick_Init(80000, finishRelease);
	setupNFCBoard();
}

int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
	DisableInterrupts();
	ST7735_InitR(INITR_REDTAB);
	initPortF();
	initPortE();
	initNFC();
	Buttons_Init(Button_Handler);
	setupCoinSelector(Coin_Handler);
	EnableInterrupts();
	
	
	//drawScreen(currentState);
	int connected = 0, transferred = 0;

	currentState = 0;
	drawScreen(currentState);
	
	while(1) {
		//confirm message detected
		if(isTagConnected() || connected) {
			
			//app will not write until it detects a number
			if(!connected) {
				GPIO_PORTE_DATA_R ^= 0x20;
				currentState = 1;
				drawScreen(currentState);	
				//wait until done inserting coins, then write tag and continue
				while(currentState == 1){				}
				//currentState = 2;
				drawScreen(currentState);	
					
				writeValue(sessionAmount);
				releaseTag();
				transferred = 1;
			}
			//number is written and detected by app which writes done message
			connected = 1;
			
			//data has been transferred and ack received from app
			if(isTransferred()) {
				//state 3 -> transfer ack by app
				currentState = 3;
				drawScreen(currentState);	
				//reset state
				GPIO_PORTE_DATA_R ^= 0x20;
				connected = 0;
				transferred = 0;
				//start timeout here, set currentState to 0 when done
				//wait until timeout to start new transaction loop
				while(currentState == 3){}
				drawScreen(currentState);	
				sessionAmount = 0;
				//currentState = 0;
				releaseTag();
			}
		}
		
		if(!connected || transferred) {
			releaseTag();
		}
		
		for(int temp = 0; temp < 10000; temp++) {
			for(int counta = 0; counta < 2000; counta++) {
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
