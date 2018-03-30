// Music.c
//SysTick handles frequency changes

#include <stdint.h>
#include "Dac.h"
#include "../inc/tm4c123gh6pm.h"
#include "SysTick.h"
#include "Music.h"
#include "Timer0A.h"

int m_pointer = 0;
Music cMusic;
extern int space;

void Sound_Init(uint32_t period){
	DAC_Init();          // Port B is DAC
  m_pointer = 0;
	SysTick_Init(period);
	
	//not correct, need to change to duration of shortest note
	Timer0A_Init(20000000);
}

void Sound_Play(uint32_t period, Music m){
	cMusic = m;
	Sound_Init(period);
}

void Sound_FChange(uint32_t period) {
	SysTick_Init(period);
}

void Sound_IChange(Instrument nI) {
	cMusic.i = nI;
}

void Sound_Stop() {
	SysTick_Disable();
	stopMusic();
}

void Sound_Start() {
	SysTick_Enable();
	startMusic();
}

void Sound_Rewind() {
	Sound_Stop();
	rewindMusic();
	Sound_Start();
}

unsigned short GetWave() {
	return cMusic.i.sound[m_pointer%64];
}

void SysTick_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x04;       // toggle PF2
	GPIO_PORTF_DATA_R ^= 0x04;       // toggle PF2

	DAC_Out(GetWave()/2);
	m_pointer = m_pointer+1; 
	GPIO_PORTF_DATA_R ^= 0x04;       // toggle PF2
}
