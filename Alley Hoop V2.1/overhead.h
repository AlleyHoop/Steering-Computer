/*
*This is a header for headers. These defines and includes need to added to pretty much all headers, so to save space, increase readability and allow for better modifiability, they were all placed in this file, and only this file is
*included in the headers
*/


#ifndef OVERHEAD_H_
#define OVERHEAD_H_

#define F_CPU 16000000L


#include <avr/io.h>			//massive define lists with registries
#include <avr/interrupt.h>
#include <avr/io.h>			
#include <util/delay.h>		//allows usage of _delay_ms()
#include "IOPins.h"
#include "USART.h"
#include "SPI.h"
#include "mcp_can.h"
#include "Arduino.h"


//comment this piece in or out to enable debug mode, which prints all the diagnostic informations
#define DEBUG_MODE

#define HIGH 1
#define LOW 0
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define A4 4
#define A5 5
#define A6 6
#define A7 7
#define A8 8 
#define A9 9
#define A10 10
#define A11 11
#define A12 12
#define A13 13
#define A14 14
#define A15 15




#endif /* DEFINES_H_ */