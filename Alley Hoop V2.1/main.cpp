#define F_CPU 16000000UL
#include <avr/io.h>			//massive define lists with registries
#include <avr/interrupt.h>	//allows usage of interrupt
#include <util/delay.h>		//allows usage of _delay_ms()
#include "overhead.h"		//defines and standard includes
#include "Drive.h"			//takes care of driving the car

USART Serial;				//global Serial
MCP_CAN CAN_Curt(47);		//the Curtiss CAN BUS

extern uint16_t low_voltage;
extern uint16_t engine_rpm;
extern uint16_t engine_temp;
extern uint16_t control_temp;
extern uint16_t high_voltage;

//comment this piece in or out to enable debug mode, which prints all the diagnostic informations
#define DEBUG_MODE 1

void printinfo();
void updateCurtiss();

//canbus print variables
long unsigned int rxId_Curt;
unsigned char len_Curt;
unsigned char rxBuf_Curt[8];
char msgString_Curt[128]; 

int main(void){
	Serial.println("Alley Hoop V2.1");
	initIO();															//initializes the IO pins
	initDrive();														//detect steering modus

	if(CAN_Curt.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)		//initialize the CAN BUS
		Serial.println("MCP2515 Initialized Successfully!");
	else
		Serial.println("Error Initializing MCP2515...");
	CAN_Curt.setMode(MCP_NORMAL);										// Set operation mode to normal so the MCP2515 sends acks to received data.

	while (1){				//main program loop	
		updateCurtiss();		//update the values retrieved from the Curtis
		drive();				//update the values send to various components
		#ifdef DEBUG_MODE
			printinfo();			//print info in the terminal, only when debug mode is enabled
		#endif
	}
}

#ifdef DEBUG_MODE									//only spam terminal when in debug mode
void printinfo(){

	Serial.print("DM: ");
	Serial.print(drivemode);
	Serial.print(" drive dir: ");
	Serial.print(driving_direction);
	Serial.print(" engine_dv = ");
	Serial.print(engine_dv);
	Serial.print(" brake_dv = ");
	Serial.print(braking_dv);
	Serial.print(" braking_ov = ");
	Serial.print(braking_ov);

	Serial.print(" steering_dv = ");
	Serial.print(steering_dv);
	Serial.print(" steering_cv = ");
	Serial.print(steering_cv);
	Serial.print(" steering_ov = ");
	Serial.print(steering_ov);
	Serial.print(" steering_delta = ");
	Serial.print(steering_delta);
	Serial.print(" low volt = ");
	Serial.print(low_voltage);
	Serial.print(" high volt = ");
	Serial.print(high_voltage);
	Serial.print(" rpm = ");
	Serial.print(engine_rpm);
	Serial.print(" temp = ");
	Serial.println(engine_temp);
}
#endif

void updateCurtiss(){
	CAN_Curt.readMsgBuf(&rxId_Curt, &len_Curt, rxBuf_Curt);	// Read data: len = data length, buf = data byte(s)
	if((rxId_Curt & 0x40000000) == 0x40000000)					//Ignore remote requests
		return;	
	low_voltage = rxBuf_Curt[0] + (rxBuf_Curt[1] * 256);
	engine_rpm = rxBuf_Curt[2] + (rxBuf_Curt[3] * 256);
	engine_temp = rxBuf_Curt[4] + (rxBuf_Curt[5] * 256);
	control_temp = rxBuf_Curt[6] + (rxBuf_Curt[7] * 256);
	high_voltage = rxBuf_Curt[8] + (rxBuf_Curt[7] * 256);	
}
