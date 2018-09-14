#include "USART.h"

//constructor with setup

USART::USART(){
	UCSR0B = (1 << TXEN0);						// Enable de USART Transmitter
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);		// 8 data bits, 1 stop bit
	UBRR0H=0;									//write the baudrate (76800)
	UBRR0L=8;									// "
}



//write a string in the terminal
void USART::print(const char st[]) {
	for(uint8_t i = 0 ; st[i] != 0 ; i++) {		//iterate over every char in the string
		while(~UCSR0A & (1 << UDRE0));			//wait till the transmit buffer is empty
		UDR0 = (int)st[i];						//write the char in the transmit buffer
	}
}

void USART::println(const char st[]){
	this->print(st);
	this->print("\n\r");
}

//write an integer in the terminal
void USART::print(int i) {
	char buffer[16];							//create a string
	itoa(i,buffer,10);							//parse the integer into the string
	print(buffer);						//print the string
}

void USART::println(int i){
	this->print(i);
	this->print("\n\r");
}
