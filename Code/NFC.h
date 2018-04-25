/*
* This function initializes the i2c pins
*/
void setupNFCBoard(void);

void readTag(void);

void writeValue(int data);

void writeTag(uint8_t values[], int length);

/*
* This function writes to the onboard tag via i2c for transmission to an NFC connected device
*/
int sendTransaction(uint8_t array[], int size, int has_response, int response_length);

void sendRelease(void);

void finishRelease(void);
