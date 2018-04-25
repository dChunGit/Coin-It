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
	//ST7735_InitR(INITR_REDTAB);
	SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
	
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
	
	SysTick_Init(80000, finishRelease);
	Buttons_Init(Button_Handler);
	setupCoinSelector(Coin_Handler);
	setupNFCBoard();
	EnableInterrupts();
	
	//GPIO_PORTF_DATA_R ^= 0x04;  
	//readTag();
	writeValue(536);
	/*sendTransaction(killNFC, 1, 0, 0);
	sendTransaction(selectNFC, 16, 1, 5);
	
	sendTransaction(selectSystem, 10, 1, 5);
	sendTransaction(readSystemLength, 8, 1, 7);
	sendTransaction(readSystemContent, 8, 1, 23);
	sendTransaction(verifyPassword, 24, 1, 5);
	sendTransaction(setGPO2, 9, 1, 5);
	sendTransaction(selectSystem2, 10, 1, 5);
	sendTransaction(readSystemLength2, 8, 1, 7);
	sendTransaction(readSystemContent2, 8, 1, 23);
	
	//sendTransaction(selectCC, command_sizes[2], 1);
	sendTransaction(selectNDEF, 10, 1, 5);
	sendTransaction(clearNDEFFileLength, 10, 0, 5); //this works -> will clear ndef length
	sendTransaction(confirmed, 24, 0, 5); //this works somehow?
	sendTransaction(fileLength, 10, 0, 5);
	//sendTransaction(readLength, 8, 1, 7);
	//int length = processLength();
	//17 for pcb + ndef
	//sendTransaction(readFile, 8, 1, 17);
	//sendTransaction(deselect, 3, 1);
	sendRelease();*/
	//printTag();
	
	//GPIO_PORTF_DATA_R ^= 0x04;  
	
	while(1) {
		//state 0
		//setup periodic timer to read tag
		//on receive connected message -> transition state (turn off NFC RF)
		//state 1
		//wait for input from Coin Selector -> update screen
		//wait for button press from Switch -> transition state (write to tag and turn on NFC RF)
		//state 2
		//on receive completed message -> transition state
	}

}
