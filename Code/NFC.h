/*
* This function initializes the i2c pins
*/
void setupNFCBoard(void);

int readTag(void);

void writeValue(int data);

void sendRelease(void);

void finishRelease(void);
