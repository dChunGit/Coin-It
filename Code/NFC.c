/*
 * Grace Zhuang (gpz68) and David Chun (dc37875)
 * 15655, T/TH 12:30-2:00
 * TA: Josh Minor
 * Lab 11
 */
#include <stdint.h>
#include <stdarg.h>
#include "../inc/tm4c123gh6pm.h"

#define MAXRETRIES              100           // number of receive attempts before giving up

int8_t slave = 0x56;

char response[100];
char* pointer = response;
int size = 0;

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
			
			if(size == 1) {
				I2C0_MCS_R = (0|I2C_MCS_START|I2C_MCS_RUN);
				//I2C0_MCS_R = (0|I2C_MCS_START|I2C_MCS_RUN);
			} else if(a == 0) {
				I2C0_MCS_R = (0|I2C_MCS_START|I2C_MCS_RUN);
			} else {
				I2C0_MCS_R = (0|I2C_MCS_RUN);
			}
			while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
				
			if((I2C0_MCS_R&(I2C_MCS_DATACK|I2C_MCS_ADRACK|I2C_MCS_ERROR)) != 0) {
				I2C0_MCS_R = (0|I2C_MCS_STOP);
				errors++;
			}
	}
	
	I2C0_MCS_R = (0|I2C_MCS_STOP);
	while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done

	//wait for device
	if(has_response) {
		do {
			I2C0_MSA_R = (slave<<1)&0xFE;
			I2C0_MSA_R &= ~0x01;
			I2C0_MCS_R = (0|I2C_MCS_STOP|I2C_MCS_START|I2C_MCS_RUN);    // master enable
			while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
				
			//loop until slave sends ack
		} while((I2C0_MCS_R&(I2C_MCS_DATACK)) != 0);
			
		I2C0_MCS_R = (0|I2C_MCS_STOP);
		while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done

		//Read receive until no ack
		int retryCounter = 0;
		int received = 0;
		int first = 1;
		size = 0;
		
		do {
			retryCounter = 0;
			do {

				I2C0_MSA_R = (slave<<1)&0xFE;    // MSA[7:1] is slave address
				I2C0_MSA_R |= 0x01;              // MSA[0] is 1 for receive
				
				if(first) {
					I2C0_MCS_R = (0|I2C_MCS_ACK|I2C_MCS_START|I2C_MCS_RUN);    // master enable
				} else {
					I2C0_MCS_R = (0|I2C_MCS_ACK|I2C_MCS_RUN);    // master enable
				}
				while(I2C0_MCS_R&I2C_MCS_BUSY){};				// wait for transmission done
				
				response[size] = (I2C0_MDR_R);
			  retryCounter = retryCounter + 1;        // increment retry counter

			} while(((I2C0_MCS_R&(I2C_MCS_ERROR)) != 0) && (retryCounter <= MAXRETRIES));
			
			first = 0;
			size++;
			received++;
			
		} while(received < response_length);
		
		I2C0_MCS_R = (0|I2C_MCS_STOP);    // master enable
		while(I2C0_MCS_R&I2C_MCS_BUSY){};// wait for transmission done
		//size++;
	}
}

void readRequest(uint8_t slave_addr, uint8_t num_of_args, ...) {
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
 //   I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);
     
    //stores list of variable number of arguments
 /*   va_list vargs;
     
    //specifies the va_list to "open" and the last fixed argument
    //so vargs knows where to start looking
    va_start(vargs, num_of_args);
     
    //put data to be sent into FIFO
    I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
     
    //if there is only one argument, we only need to use the
    //single send I2C function
    if(num_of_args == 1)
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
         
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));
         
        //"close" variable argument list
        va_end(vargs);
    }
     
    //otherwise, we start transmission of multiple bytes on the
    //I2C bus
    else
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
         
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));
         
        //send num_of_args-2 pieces of data, using the
        //BURST_SEND_CONT command of the I2C module
        for(uint8_t i = 1; i < (num_of_args - 1); i++)
        {
            //put next piece of data into I2C FIFO
            I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
            //send next data that was just placed into FIFO
            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
     
            // Wait until MCU is done transferring.
            while(I2CMasterBusy(I2C0_BASE));
        }
     
        //put last piece of data into I2C FIFO
        I2CMasterDataPut(I2C0_BASE, va_arg(vargs, uint32_t));
        //send next data that was just placed into FIFO
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));
         
        //"close" variable args list
        va_end(vargs);
    }*/
}
