#ifndef	_COMMON_H
#define	_COMMON_H

#include <ESP8266WiFi.h>




typedef union uWord {
	short int Value;
  	unsigned char Val[sizeof(short int)];
} Word;


#define MAX_STRING_LEN  80
#define MAX_STRING      200

#endif


