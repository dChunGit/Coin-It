/*
* This function initializes the i2c pins
*/
void setupNFCBoard(void);

/*
* This function writes to the onboard tag via i2c for transmission to an NFC connected device
*/
void sendTransaction(uint8_t array[], int size, int has_response, int response_length);

/*
* Function reads in the onboard tag via i2c
*/
void readRequest(uint8_t slave_addr, uint8_t num_of_args, ...);
