#define F_CPU 16000000
#include <avr/io.h>			//massive define lists with registries
#include <avr/interrupt.h>	//allows usage of interrupt
#include <util/delay.h>		//allows usage of _delay_ms()
#include "overhead.h"		//defines and standard includes
#include "Drive.h"			//takes care of driving the car
USART Serial;				//global Serial

void printinfo();

int main(void){
	Serial.println("Alley Hoop V2.1");
	//setup canbus
	initIO();				//initializes the IO pins
	initDrive();
	//detect steering modus
	while (1){				//main program loop
		printinfo();
		drive();
		_delay_ms(10);
	}
}

void printinfo(){
	Serial.print("driving mode: ");
	Serial.print(drivemode);
	//Serial.print(" driving direction =  ");
	//Serial.print(driving_direction);
	//Serial.print(" engine_dv =  ");
	//Serial.print(engine_dv);
	//Serial.print(" Gassign. =  ");
	//Serial.print(engine_ov);
	//Serial.print(" brake_sp = ");
	//Serial.print(braking_dv);
	//Serial.print(" brake_uit = ");
	//Serial.print(brake_ov);
	Serial.print(" steering_dv =  ");
	Serial.print(steering_dv);
	Serial.print(" steering_cv =  ");
	Serial.print(steering_cv);
	Serial.print(" steering_op =  ");
	Serial.print(steering_op);
		Serial.print(" delta =  ");
		Serial.println(steering_delta);
}
