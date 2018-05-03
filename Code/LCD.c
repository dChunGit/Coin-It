/*
 * Grace Zhuang (gpz68) and David Chun (dc37875)
 * 15655, T/TH 12:30-2:00
 * TA: Josh Minor
 * Lab 4
 */

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"  
#include "Bitmaps.h"

extern int sessionAmount;
extern int currentState;

void drawAmount() {
	int temp = sessionAmount;
	int hundredth = temp % 10;
	temp = temp/10;
	int tenth = temp % 10;
	temp = temp/10;
	int first = temp % 10;
	int second = temp/10;
	
	/*
	ST7735_DrawBitmap(10, 80, dollar, 20, 40);
	if (second != 0) {
			ST7735_DrawBitmap(30, 80, numbers[second], 20, 40);
	}
	ST7735_DrawBitmap(50, 80, numbers[first], 20, 40);
	ST7735_DrawBitmap(70, 80, period, 20, 40);
	ST7735_DrawBitmap(85, 80, numbers[tenth], 20, 40);
	ST7735_DrawBitmap(105, 80, n, 20, 40);
	*/
	
	ST7735_SetCursor(7, 7);
	char string[] = {'$', second + '0', first + '0', '.', tenth + '0', hundredth + '0',};
	ST7735_OutString(string);
}

void drawScreen(int state) {
	
	if (state == 0) {
		ST7735_DrawBitmap(0, 160, home, 128, 160);
	}
	
	else if (state == 1) {
		ST7735_DrawBitmap(0, 160, coininsert, 128, 160);
		drawAmount();
	}
	
	else if (state == 2) {
		ST7735_DrawBitmap(0, 160, confirm, 128, 160);
		drawAmount();
	}
	
	else if (state == 3) {
		ST7735_DrawBitmap(0, 160, final, 128, 160);
		drawAmount();
		int temp = 0;
		int temp2 = 0;
		while (temp2 < 100) { // wait arbitrary amount of time before returning to home screen
			temp++;
			if (temp >= 500000) {
				temp = 0;
				temp2++;
			}
		}
		sessionAmount = 0;
		//state = 0;
		currentState = 0;
		drawScreen(currentState);
	}
}

void printTag(char response_Tag[]) {
	for(int a = 0; a < 9; a++) {
		char printout = response_Tag[8+a];
		ST7735_OutChar(response_Tag[13+a]);
	}
}

void updateAmount(int amount) {
	
}
