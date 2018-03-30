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
//state 0: begin transaction, state 1: transaction in progress, state 2: transaction finished
int currentState = 0;

void Button_Handler(int button){
	switch(button) {
	}
}

void Coin_Handler() {
	
}

int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
	DisableInterrupts();

	Buttons_Init(Button_Handler);
	setupCoinSelector(Coin_Handler);
	setupNFCBoard();
	EnableInterrupts();
	
	while(1) {
		//wait for input from Coin Selector
		//wait for button press from Switch
		//update screen accordingly
	}

}
