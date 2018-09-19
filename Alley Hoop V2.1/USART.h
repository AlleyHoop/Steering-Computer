/*
* USART.h
*This function takes care of the serial connection over the USB port for debugging purposes
*	print(char* or int)		write one or more characters in the Serial
*
* Created: 7-9-2018 14:01:55
* Author: Jur Hoekstra
*/

#ifndef __USART_H__
#define __USART_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class USART{
	public:
	USART();								//regular constructor, uses baud rate of 9600
	~USART(){};								//deconstructor
	void print(const char[]);						//write one or more characters in the Serial
	void print(int);						//write an integer
	void println(const char[]);						//write one or more characters in the Serial on a new line
	void println(int);						//write an integer on a new line
	
};
#endif //__USART_H__
