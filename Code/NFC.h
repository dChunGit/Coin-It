/*
* This function initializes the i2c pins
*/
void setupNFCBoard(void);

int readTag(int type);

int isTagConnected(void);

int isTransferred(void);

void writeValue(int data);

void holdTag(void);

void releaseTag(void);

/*void sendRelease(void);

void finishRelease(void);*/
