#include "common.h"

#define MAX_OSC_BUFFER_SIZE  200

typedef struct s_OSC {
  char buf[MAX_OSC_BUFFER_SIZE];
  char *pData;
  unsigned int PacketSize;
} OscBuffer;


void PrepareOSC(OscBuffer *TheBuffer, char *OscAddress, char TypeTag, uint8_t Slots);
void ShortToBigEndian(char *Dest, short int Val);
void WordToBigEndian(char *Dest, Word TheWord);
void FloatToBigEndian(char *Dest, float *TheFloat);


