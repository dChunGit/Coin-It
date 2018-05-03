/*
 * Grace Zhuang (gpz68) and David Chun (dc37875)
 * 15655, T/TH 12:30-2:00
 * TA: Josh Minor
 * Lab 11
 */
#include <stdint.h>
#include <stdarg.h>
#include "../inc/tm4c123gh6pm.h"
#include "SysTick.h"
#include "NFC.h"
#include "Main.h"
#include "CRC.h"

#define MAXRETRIES              100           // number of receive attempts before giving up

char response[50];
char* pointer = response;
int response_pointer = 0;

uint8_t ndef[30];
uint8_t ndef_length[] = {0x02, 0x00, 0xD6, 0x00, 0x00, 0x02, 0x00, 0x10, 0x55, 0xA6};

uint8_t readIn[] = {0x02, 0x00, 0xB0, 0x00, 0x02, 0x10, 0x48, 0x7D};

int valueLength = 0;
uint8_t confirmcheck[] = "Confirmed";
uint8_t donecheck[] = "Done";

int8_t slave = 0x56;

void setupNFCBoard(void) {
	SYSCTL_RCGCI2C_R |= 0x0001;           // activate I2C0
  SYSCTL_RCGCGPIO_R |= 0x0002;          // activate port B
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?

  GPIO_PORTB_AFSEL_R |= 0x0C;           // 3) enable alt funct on PB2,3
  GPIO_PORTB_ODR_R |= 0x08;             // 4) enable open drain on PB3 only
  GPIO_PORTB_DEN_R |= 0x0C;             // 5) enable digital I/O on PB2,3
                                        // 6) configure PB2,3 as I2C
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
  GPIO_PORTB_AMSEL_R &= ~0x0C;          // 7) disable analog functionality on PB2,3
  I2C0_MCR_R = I2C_MCR_MFE;      // 9) master function enable
  I2C0_MTPR_R = 0x03;              // 8) configure for 1 mbps clock
}

int processLength() {
	return response[2];
}

int convertToASCII(int number) {
	int temp = number;
	int multiplier = 10;
	int pointer_value = valueLength;
	int savep = 1;
	
	while(number/multiplier > 0) {
		multiplier *= 10;
		pointer_value++;
		savep++;
	}
	
	valueLength = pointer_value + 1;
	
	while(temp > 0 && pointer_value > 0) {
		int current = temp%10;
		ndef[pointer_value] = current + '0';
		pointer_value--;
		temp /= 10;
	}
	
	return savep;
}

int isTagConnected() {
	int readR = readTag(1);
	
	if(readR != 0) {
		
		for(int a = 0; a < 9; a++) {
			if(response[8+a] != confirmcheck[a]) {
				return 0;
			}
		}
		
		//holdTag();
		return 1;
	}
	
	return 0;
}

int isTransferred() {
	int readR = readTag(1);
	
	if(readR != 0) {
	
		for(int a = 0; a < 4; a++) {
			if(response[8+a] != donecheck[a]) {
				return 0;
			}
		}
		
		//holdTag();
		return 1;
	}
	
	return 0;
}

int sendTransaction(uint8_t data[], int size, int has_response, int response_length) {
	int errors = 0;
	
	for(int a = 0; a < size; a++) {
			while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for I2C ready
			I2C0_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
			I2C0_MSA_R &= ~0x01;             // MSA[0] is 0 for send
			I2C0_MDR_R = data[a]&0xFF;			
			
			if((size == 1) || (a == 0)) {
				I2C0_MCS_R = (0|I2C_MCS_START|I2C_MCS_RUN);		//start if only or first byte
			} else {
				I2C0_MCS_R = (0|I2C_MCS_RUN);									//run transmit
			}
			while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
				
			if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0) {	//detect error
				I2C0_MCS_R = (0|I2C_MCS_STOP);
				errors++;
			}
	}
	
	I2C0_MCS_R = (0|I2C_MCS_STOP);
	while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done

	//command reads response
	if(has_response) {
		//wait for device ready for receive
	/*	do {
			I2C0_MSA_R = (slave<<1)&0xFE;
			I2C0_MSA_R &= ~0x01;
			I2C0_MCS_R = (0|I2C_MCS_STOP|I2C_MCS_START|I2C_MCS_RUN);    // send request waiting for ack
			while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
				
			//loop until slave sends ack
		} while((I2C0_MCS_R&(I2C_MCS_DATACK)) != 0);*/
		for(int temp = 0; temp < 1000; temp++) {
			for(int count = 0; count < 100; count++) {
			}
		}
		
		//stop communication
		I2C0_MCS_R = (0|I2C_MCS_STOP);
		while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done

		//Read receive until no ack
		int retryCounter = 0;
		int received = 0;
		int first = 1;
		response_pointer = 0;
	
		do {
			retryCounter = 0;
			//send until acknowledged or hit max retries
			do {
				I2C0_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
				I2C0_MSA_R |= 0x01;              // MSA[0] is 1 for receive
				
				if(first) {
					I2C0_MCS_R = (0|I2C_MCS_ACK|I2C_MCS_START|I2C_MCS_RUN);    // only start on first send
				} else {
					I2C0_MCS_R = (0|I2C_MCS_ACK|I2C_MCS_RUN);    // run transmission -> repeated start not supported
				}
				while(I2C0_MCS_R&I2C_MCS_BUSY){};				// wait for transmission done
				
				response[response_pointer] = (I2C0_MDR_R);					//read in data
			  retryCounter = retryCounter + 1;        // increment retry counter

			} while(((I2C0_MCS_R&(I2C_MCS_ERROR)) != 0) && (retryCounter <= MAXRETRIES));
			
			first = 0;
			response_pointer++;
			received++;
			
		} while(received < response_length);
		
		//stop communication
		I2C0_MCS_R = (0|I2C_MCS_STOP);    // master enable
		while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
	} else {
		for(int temp = 0; temp < 1000; temp++) {
			for(int count = 0; count < 100; count++) {
			}
		}
	}
	
	if(errors > 0) {
		return -1;
	}
	
	return 0;
	
}
/*
void sendRelease() {
		I2C0_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
		I2C0_MSA_R |= 0x01;              // MSA[0] is 1 for receive
		I2C0_MCS_R = (0|I2C_MCS_START|I2C_MCS_RUN);    // only start on first send

		GPIO_PORTB_DATA_R &= ~0x04;
		SysTick_Enable();
	
}

void finishRelease() {
		I2C0_MCS_R = (0|I2C_MCS_STOP);    // master enable
		while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done

}*/

void holdTag() {
		sendTransaction(killNFC, 1, 0, 0);
}

void releaseTag() {
		sendTransaction(deselect, 3, 1, 5);
}

void writeTag(uint8_t values[], int length) {
	sendTransaction(killNFC, 1, 0, 0);
	sendTransaction(selectNFC, 16, 1, 5);
	sendTransaction(selectNDEF, 10, 1, 5);
	sendTransaction(clearNDEFFileLength, 10, 0, 5); //this works -> will clear ndef length
	//sendTransaction(confirmed, 24, 0, 5); //this works somehow?
	sendTransaction(values, length, 0, 0);
	//sendTransaction(fileLength, 10, 0, 5);
	sendTransaction(ndef_length, 10, 0, 5);
	//sendTransaction(deselect, 3, 1, 5);
	//sendRelease();
}

void writeValue(int data) {
	for(valueLength = 0; valueLength < 13; valueLength++) {
		ndef[valueLength] = header[valueLength];
	}
	int length = convertToASCII(data);
	ndef[8] = length + 3;
	ndef[5] = ndef[8] + 4;
	uint16_t crc = M24SR_ComputeCrc(ndef, valueLength);
	ndef[valueLength] = crc&0xFF;
	valueLength++;
	ndef[valueLength] = (crc>>8)&0xFF;
	valueLength++;
	//concatenate header, ndef, and crc
	ndef_length[7] = ndef[5];
	crc = M24SR_ComputeCrc(ndef_length, 8);
	ndef_length[8] = crc&0xFF;
	ndef_length[9] = (crc>>8)&0xFF;
	int a = 0;
	writeTag(ndef, valueLength);
	//writeTag(confirmed, 24);
}

int readTag(int type) {
	int success = -1, count = 0;
	
	//sendTransaction(killNFC, 1, 0, 0);
	do {
		success = sendTransaction(getICSession, 1, 0, 0);
		count++;
	} while(success != 0 && count < 10);
	
	if(success == 0) {
		sendTransaction(selectNFC, 16, 1, 5);
		
		sendTransaction(selectNDEF, 10, 1, 5);
		sendTransaction(readLength, 8, 1, 7);
		int length = processLength();
		readIn[5] = length&0xFF;
		//17 for pcb + ndef
		uint16_t crc = M24SR_ComputeCrc(readIn, 6);
		readIn[6] = crc&0xFF;
		readIn[7] = (crc>>8)&0xFF;
		/*if(type) {
			sendTransaction(readFile, 8, 1, length+1);
		} else {
			sendTransaction(readNumber, 8, 1, length+1);
		}*/
		sendTransaction(readIn, 8, 1, length+1);
		if(type != 1) {
			sendTransaction(deselect, 3, 1, 5);
		}
		//sendRelease();
		return 1;
	}
	return 0;

}

void writeStartedTransfer() {
	//need to change length of started write command
	ndef_length[7] = started[5];
	writeTag(started, 24);
}
