#ifndef __TIMER0AINTS_H__ // do not include more than once
#define __TIMER0AINTS_H__

// ***************** Timer0A_Init ****************
// Activate Timer0A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0A_Init(uint32_t period);

#endif // __TIMER0AINTS_H__
void rewindMusic(void);
void stopMusic(void);
void startMusic(void);
void rewindMusic(void);
