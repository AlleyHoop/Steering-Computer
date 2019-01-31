/* 
* wiring.h
copied from the Arduino library, takes care of various functions
*
* Created: 19-12-2018 10:02:48
* Author: Jur Hoekstra
*/

#include "overhead.h"


#ifndef __WIRING_H__
#define __WIRING_H__

#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))



unsigned long micros();			//get the time in microseconds
unsigned long millis();			//get the time in milliseconds
void init();					//init time stuff


#endif //__WIRING_H__
