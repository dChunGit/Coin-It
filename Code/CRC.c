/*
 * Grace Zhuang (gpz68) and David Chun (dc37875)
 * 15655, T/TH 12:30-2:00
 * TA: Josh Minor
 * Lab 11
 */
#include <stdint.h>

static uint16_t M24SR_UpdateCrc(uint8_t ch, uint16_t *lpwCrc) {
	ch = (ch^(uint8_t)((*lpwCrc) & 0x00FF));
	ch = (ch^(ch<<4));
	*lpwCrc = (*lpwCrc >> 8)^((uint16_t)ch << 8)^((uint16_t)ch<<3)^((uint16_t)ch>>4);
	
	return(*lpwCrc);
}

static uint16_t M24SR_ComputeCrc(uint8_t *Data, uint8_t Length) {
	uint8_t chBlock;
	uint16_t wCrc;
	wCrc = 0x6363; // ITU-V.41
	
	do {
		chBlock = *Data++;
		M24SR_UpdateCrc(chBlock, &wCrc);
	} while (--Length);

	return wCrc ;
}
