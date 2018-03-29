/*
 * Grace Zhuang (gpz68) and David Chun (dc37875)
 * 15655, T/TH 12:30-2:00
 * TA: Josh Minor
 * Lab 11
 */
 
void (*CoinTask)(void);   // user function

 
void setupCoinSelector(void(*task)(void)) {
	CoinTask = task;
}

void GPIOPortD_Handler(void) {
	
}
