/*
* This function initializes the i2c pins
*/
void setupNFCBoard(void);

/*
* This function writes to the onboard tag via i2c for transmission to an NFC connected device
*/
void sendTransaction(void);

/*
* Function reads in the onboard tag via i2c
*/
void readRequest(void);
