#define F_CPU 16000000
#include <avr/io.h>			//massive define lists with registries
#include <avr/interrupt.h>	//allows usage of interrupt
#include <util/delay.h>		//allows usage of _delay_ms()
#include "overhead.h"		//defines and standard includes
#include "Drive.h"			//takes care of driving the car
USART terminal;				//global terminal

//void printinfo();

int main(void){
	while(1){};
	terminal.println("Alley Hoop V2.0");	
							//setup canbus
	initIO();				//initializes the IO pins
	initDrive();
	digitalWrite(12,1);
	digitalWrite(13,1);
	//detect steering modus
		while (1){				//main program loop
		_delay_ms(50);
		analogWrite(6,OCR0A+10);

		OCR0B+=10;
		OCR1A+=10;
		OCR1B+=10;
		OCR2A+=10;
		OCR2B+=10;
		_delay_ms(50);						//dont do this kids
	}
}

void printinfo(){	
	     terminal.print("mode_forward =  ");
	     terminal.print(mode_forward);
	     terminal.print("mode_reverse =  ");
	     terminal.print(mode_reverse);
	     terminal.print("engine_sp =  ");
	     terminal.print(engine_sp);
	     terminal.print("Gassign. =  ");
	     terminal.print(engine_op);
	     terminal.print("brake_sp = ");
	     terminal.print(brake_sp);
	     terminal.print("brake_uit = ");
	     terminal.print(brake_op);
	     terminal.print("Stuur_sp =  ");
	     terminal.print(steering_sp);
	     terminal.print("Stuurhoek =  ");
	     terminal.print(steering_pv);
	     terminal.print("Stuuruit =  ");
	     terminal.println(steering_op);	
}
