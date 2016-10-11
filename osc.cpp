// Basic handling of an OSC packet

//#include <energia.h>
#include <stdio.h>
#include <string.h>
#include "osc.h"
#include "common.h"

void PrepareOSC(OscBuffer *TheBuffer, char *OscAddress, char TypeTag, uint8_t Slots)
{
  int i;
  int size = 0;
  char *pBuf;
	
  pBuf = TheBuffer->buf;
  size = strlen(OscAddress);
  strcpy(pBuf, OscAddress);
  pBuf += size;
 
  // We can't stop on an aligned %4, as we need at least one zero terminator in the address
  if(!(size%4))
  {
    *pBuf = '\0';
    size++;
    pBuf++;
  }
  
  // 4 byte padding / suffing
  while(size%4)
  {
    *pBuf = '\0';
    size++;
    pBuf++;
  }	
  // adds the comma char (separator for typetags)
  *pBuf = ',';
  pBuf++;
  size++;
  // adds the type tags
  for(i = 0 ; i < Slots ; i++)
  {
    *pBuf = TypeTag;
    pBuf++;  
  }
  size += Slots;
  
  // We can't stop on an aligned %4, as we need at least one zero terminator in the address
  if(!(size%4))
  {
    *pBuf = '\0';
    size++;
    pBuf++;
  }	
  // 4 byte padding / stuffing
  while(size%4)
  {
    *pBuf = '\0';
    pBuf++;
    size++;
  }

  // Stores where the actual data start
  TheBuffer->pData = pBuf;
  
  // Adds size of data
  size += Slots*4;
  pBuf += Slots*4;
	
  // Dumb check as at that point it's supposed to be 4-byte aligned
  while(size%4)
  {
    size++;
    pBuf++;
  }
  TheBuffer->PacketSize = size;
  *(pBuf+1) = '\0';
  //Serial.print("OSC packet size = ");
  //Serial.println(size);
}

void ShortToBigEndian(char *Dest, short int Val)
{
  // Sign padding
  if(Val < 0)
  {
      Dest[0] = 0xFF;
      Dest[1] = 0xFF;
  }
  else
  {
      Dest[0] = 0;
      Dest[1] = 0;
  }
  Dest[2] = (unsigned char)(Val >> 8);
  Dest[3] = (unsigned char)(Val & 0x00FF);
}

void WordToBigEndian(char *Dest, Word TheWord)
{
  // Sign padding / extension
  if(TheWord.Value < 0)
  {
      Dest[0] = 0xFF;
      Dest[1] = 0xFF;
  }
  else
  {
      Dest[0] = 0;
      Dest[1] = 0;
  }
  Dest[2] = TheWord.Val[1];
  Dest[3] = TheWord.Val[0];
}


void FloatToBigEndian(char *Dest, float *TheFloat)
{
  char *pF = (char*)TheFloat;
  Dest[0] = pF[3];
  Dest[1] = pF[2];
  Dest[2] = pF[1];
  Dest[3] = pF[0];

}

