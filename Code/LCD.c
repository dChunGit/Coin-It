/*
 * Grace Zhuang (gpz68) and David Chun (dc37875)
 * 15655, T/TH 12:30-2:00
 * TA: Josh Minor
 * Lab 4
 */

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"  

extern int sessionAmount;

void drawScreen(int state) {
	
	if (state == 0) {
		ST7735_FillScreen(0);                 // set screen to black
		ST7735_SetCursor(0, 0);
		ST7735_OutString("Welcome! Please place your phone on the stand to begin transaction.");
	}
	
	else if (state == 1) {
		ST7735_FillScreen(0);                 // set screen to black
		ST7735_SetCursor(0, 0);
		ST7735_OutUDec(sessionAmount);
	}
	
	else if (state == 2) {
		ST7735_FillScreen(0);
		ST7735_SetCursor(0, 0);
		ST7735_OutString("Deposit ");
		ST7735_OutUDec(sessionAmount);
		ST7735_OutString(" into your account?");
	}
	
	else if (state == 3) {
		ST7735_FillScreen(0);                 // set screen to black
		ST7735_SetCursor(0, 0);
		ST7735_OutUDec(sessionAmount); // TO-DO: modify to make it into a dollar value
		ST7735_OutString(" has been deposited into your account. Thank you for using CoinIt. Don't forget your phone!");
		
		int temp = 0;
		while (temp < 10000) { // wait arbitrary amount of time before returning to home screen
			temp++;
		}
		state = 0;
	}
}

void updateAmount(int amount) {
	
}
