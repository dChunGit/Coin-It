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

#define MAXRETRIES              100           // number of receive attempts before giving up

char response[50];
char* pointer = response;
int response_pointer = 0;

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

void sendTransaction(uint8_t data[], int size, int has_response, int response_length) {
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
		/*do {
			I2C0_MSA_R = (slave<<1)&0xFE;
			I2C0_MSA_R &= ~0x01;
			I2C0_MCS_R = (0|I2C_MCS_STOP|I2C_MCS_START|I2C_MCS_RUN);    // send request waiting for ack
			while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
				
			//loop until slave sends ack
		} while((I2C0_MCS_R&(I2C_MCS_DATACK)) != 0);
		
		//stop communication
		I2C0_MCS_R = (0|I2C_MCS_STOP);
		while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done*/
		for(int wait = 0; wait < 100; wait++) {
			for(int temp = 0; temp < 10; temp++) {
				
			}
		}
		

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
	}
	
}

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

}

