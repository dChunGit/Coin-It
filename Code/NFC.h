/*
* This function initializes the i2c pins
*/
void setupNFCBoard(void);

/*
* This function writes to the onboard tag via i2c for transmission to an NFC connected device
*/
void sendTransaction(uint8_t array[], int size, int has_response, int response_length);

void sendRelease(void);

void finishRelease(void);
