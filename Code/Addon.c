
#include <stdint.h>
#include <stdio.h>
#include "../inc/tm4c123gh6pm.h"
#include <string.h>
#include "ST7735.h"
#include <time.h>
#include "ADCSWTrigger.h"

#define MAX_RECV_BUFF_SIZE  1024
volatile uint32_t ADCvalue;

char number[3];
		
void displayADC(int output) {
		ST7735_SetCursor(0, 1);
		ST7735_OutString("Voltage: ");
		ST7735_OutUDec(output);
		ST7735_OutString("V");
}

void displayBuffer(char* buffer) {
	ST7735_FillScreen(ST7735_Color565(0,0,0));
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Temp: ");
	for(int a = 0; a < MAX_RECV_BUFF_SIZE - 4; a++) {
		if(buffer[a] == 't' && buffer[a+1] == 'e' && buffer[a+2] == 'm' && buffer[a+3] == 'p') {
				int loc = a + 6;
				while(buffer[loc] != ',') {
					ST7735_OutChar(buffer[loc]);
					loc++;
				}
				break;
		}
	}
	ST7735_OutString("F");
	
}

void displayTime(struct tm* cTm) {
		ST7735_SetCursor(0, 1);
		ST7735_OutString("Time: ");
		ST7735_OutUDec(cTm->tm_hour);
		ST7735_OutString(": ");
		ST7735_OutUDec(cTm->tm_min);
}

int convertToNumber(char* time) {
		int number = 0;
		for(int a = 0; a < 10; a++) {
				number *= 10;
				number += time[a] - '0';
		}
		return number;
}

char* convertToString(int output) {
		int interim = (330*output)/4096;
		int a = 2;
		while(interim > 0 && a >= 0) {
				int current = interim%10;
				number[a] = current + '0';
				a--;
				interim /= 10;
		}
		
		return number;
}

void parseTime(char* buffer) {
		char time[11];
		int i = 0;
		for(int a = 0; a < MAX_RECV_BUFF_SIZE - 2; a++) {
			if(buffer[a] == 'd' && buffer[a+1] == 't') {
					int loc = a + 4;
					while(buffer[loc] != ',') {
							time[i] = buffer[loc];
							i++;
							loc++;
					}
					time[i] = '\0';
					break;
			}
		}
		
		int epoch_number = convertToNumber(time);
		time_t cTime = (time_t) epoch_number;
		struct tm* cTM = localtime(&cTime);
		displayTime(cTM);
}

char* getADC() {
	ADCvalue = ADC0_InSeq3();
	ST7735_SetCursor(0,1);
	ST7735_OutUDec(ADCvalue);
	return convertToString(ADCvalue);
}
