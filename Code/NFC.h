/*
* This function initializes the i2c pins
*/
void setupNFCBoard(void);

int readTag(void);

void writeValue(int data);

void sendRelease(void);

int isTagConnected(void);

void finishRelease(void);
