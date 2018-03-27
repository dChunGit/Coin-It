#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"
#include "Timer0A.h"
#include "Music.h"
#include "Switch.h"
#include "Dac.h"

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

const unsigned short Trumpet[64]={
  987, 1049, 1090, 1110, 1134, 1160, 1139, 1092, 1070, 1042, 1035, 1029, 1008, 1066, 1150, 1170, 1087, 
	915, 679, 372, 151, 558, 1014, 1245, 1260, 1145, 1063, 984, 934, 960, 1027, 1077, 1081, 1074, 1064, 
	1042, 1010, 974, 968, 974, 994, 1039, 1094, 1129, 1125, 1092, 1056, 1056, 1082, 1059, 1046, 1058, 
	1061, 1045, 1034, 1050, 1094, 1112, 1092, 1063, 1053, 1065, 1052, 992};

const unsigned short Oboe[64]={
	1024, 1024, 1014, 1008, 1022, 1065, 1093, 1006, 858, 711, 612, 596, 672, 806, 952, 1074, 1154, 1191, 
	1202, 1216, 1236, 1255, 1272, 1302, 1318, 1299, 1238, 1140, 1022, 910, 827, 779, 758, 757, 782, 856, 
	972, 1088, 1177, 1226, 1232, 1203, 1157, 1110, 1067, 1028, 993, 958, 929, 905, 892, 900, 940, 1022, 
	1125, 1157, 1087, 965, 836, 783, 816, 895, 971, 1017};

const unsigned short Bassoon[64]={
	1068, 1169, 1175, 1161, 1130, 1113, 1102, 1076, 1032, 985, 963, 987, 1082, 1343, 1737, 1863, 
	1575, 1031, 538, 309, 330, 472, 626, 807, 1038, 1270, 1420, 1461, 1375, 1201, 1005, 819, 658, 
	532, 496, 594, 804, 1055, 1248, 1323, 1233, 1049, 895, 826, 826, 850, 862, 861, 899, 961, 1006, 
	1023, 1046, 1092, 1177, 1224, 1186, 1133, 1098, 1102, 1109, 1076, 1027, 1003};

const unsigned short Flute[64]={
	2014, 2504, 2748, 3096, 3396, 3594, 3650, 3594, 3350, 3124, 2766, 2438, 2184, 2014, 1826, 1780, 
	1666, 1694, 1620, 1554, 1488, 1348, 1196, 1102, 1018, 952, 990, 1066, 1178, 1318, 1516, 1752, 
	2014, 2504, 2748, 3096, 3396, 3594, 3650, 3594, 3350, 3124, 2766, 2438, 2184, 2014, 1826, 1780, 
	1666, 1694, 1620, 1554, 1488, 1348, 1196, 1102, 1018, 952, 990, 1066, 1178, 1318, 1516, 1752};

const unsigned short Wave[64]={
	2048, 2224, 2399, 2571, 2737, 2897, 3048, 3190, 3321, 3439, 3545, 3635, 3711, 3770, 3813, 3839, 
	3848, 3839, 3813, 3770, 3711, 3635, 3545, 3439, 3321, 3190, 3048, 2897, 2737, 2571, 2399, 2224, 
	2048, 1872, 1697, 1525, 1359, 1199, 1048, 906, 775, 657, 551, 461, 385, 326, 283, 257, 248, 257, 
	283, 326, 385, 461, 551, 657, 775, 906, 1048, 1199, 1359, 1525, 1697, 1872};


Note notes[] = {
	{.pitch = C0, .length = 1},
	{.pitch = F, .length = 3},
	{.pitch = C0, .length = 1},
	{.pitch = F, .length = 1},
	{.pitch = C0, .length = 1},
	{.pitch = F, .length = 1},
	{.pitch = G, .length = 1},
	{.pitch = A, .length = 4},
	{.pitch = F, .length = 4},
	{.pitch = BF, .length = 3},
	{.pitch = BF, .length = 1},
	{.pitch = F, .length = 2},
	{.pitch = G, .length = 2},
	{.pitch = A, .length = 6},
	{.pitch = F, .length = 3},
	{.pitch = C0, .length = 1},
	{.pitch = F, .length = 1},
	{.pitch = C0, .length = 1},
	{.pitch = F, .length = 1},
	{.pitch = G, .length = 1},
	{.pitch = A, .length = 4},
	{.pitch = F, .length = 2},
	{.pitch = A, .length = 2},
	{.pitch = A, .length = 2},
	{.pitch = G, .length = 2},
	{.pitch = G, .length = 2},
	{.pitch = A, .length = 2},
	{.pitch = G, .length = 2},
	{.pitch = G, .length = 3},
	{.pitch = G, .length = 1},
	{.pitch = GF, .length = 1},
	{.pitch = G, .length = 1},
	{.pitch = A, .length = 1},
	{.pitch = G, .length = 1},
	{.pitch = F, .length = 4},
	{.pitch = C0, .length = 2},
	{.pitch = C0, .length = 2},
	{.pitch = BF, .length = 2},
	{.pitch = BF, .length = 2},
	{.pitch = F, .length = 1},
	{.pitch = F, .length = 1},
	{.pitch = G, .length = 1},
	{.pitch = G, .length = 1},
	{.pitch = A, .length = 6},
	{.pitch = D, .length = 3},
	{.pitch = E, .length = 1},
	{.pitch = F, .length = 1},
	{.pitch = E, .length = 1},
	{.pitch = F, .length = 1},
	{.pitch = D, .length = 1},
	{.pitch = C0, .length = 4},
	{.pitch = F, .length = 2},
	{.pitch = G, .length = 2},
	{.pitch = A, .length = 2},
	{.pitch = BF, .length =2},
	{.pitch = A, .length = 2},
	{.pitch = G, .length = 2},
	{.pitch = F, .length = 6}
};

int state = 0, c_instrument = 0;

void Button_Handler(int button){
	switch(button) {
		case 0: {
			//play
			if (state == 0) {
				state = 1;
				Sound_Start();
			}
			
			// pause
			else {
				state = 0;
				Sound_Stop();
			}
		} break;
		case 1: {
			//rewind
			Sound_Rewind();
		} break;
		case 2: {
			//mode
			c_instrument = (c_instrument+1)%5;
			
			switch (c_instrument) {
				case 0: {
					Instrument t_int = {.sound = Flute};
					Sound_IChange(t_int);
				} break;
				case 1: {
					Instrument t_int = {.sound = Trumpet};
					Sound_IChange(t_int);
				} break;
				case 2: {
					Instrument t_int = {.sound = Bassoon};
					Sound_IChange(t_int);
				} break;
				case 3: {
					Instrument t_int = {.sound = Oboe};
					Sound_IChange(t_int);
				} break;
				case 4: {
					Instrument t_int = {.sound = Wave};
					Sound_IChange(t_int);
				} break;
				default: break;
			}
			
		} break;
		default: break;
	}
}

// if desired interrupt frequency is f, Timer0A_Init parameter is busfrequency/f
#define F16HZ (50000000/16)
#define F20KHZ (50000000/20000)
//debug code
int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
	DisableInterrupts();
	SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
	
	GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF

	Buttons_Init(Button_Handler);
	Instrument i = {.sound = Wave};
	Music eyes = { .notes = notes, .length = 58, .i = i};
	state = 1;
	EnableInterrupts();
	
	Sound_Play(eyes.notes[0].pitch, eyes);
	Sound_Start();
	
  while(1){
		// GPIO_PORTF_DATA_R ^= 0x04;       // toggle PF2
		//On button press, handle interrupt
  }
}
