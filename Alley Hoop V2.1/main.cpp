#include "overhead.h"																							//defines and standard includes
#include "Drive.h"																								//takes care of driving the car

USART Serial;																									//initialize Serial communication 
MCP_CAN CAN_Curt(47);																							//create the CURTIS CAN BUS

extern uint16_t low_voltage;																					//import external variables
extern uint16_t engine_rpm;
extern uint16_t engine_temp;
extern uint16_t control_temp;
extern uint16_t high_voltage;

void printinfo();																								//initialize functions
void updateCurtiss();

long unsigned int rxId_Curt;																					//initialize Canbus print variables
unsigned char len_Curt;
unsigned char rxBuf_Curt[8];
char msgString_Curt[128]; 

int main(void){
	Serial.println("Alley Hoop V2.1");
	initIO();																									//initializes the IO pins
	initDrive();																								//detect steering modus
	digitalWrite(34,LOW);																						//temporary for the old PCB, as the line isn't pulled down making it impossible to turn off the arduino
	if(CAN_Curt.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)												//initialize the CAN BUS
		Serial.println("MCP2515 Initialized Successfully!");
	else
		Serial.println("Error Initializing MCP2515...");
	CAN_Curt.setMode(MCP_NORMAL);																				//Set operation mode to normal so the MCP2515 sends acknowledge packages to received data. (currently not operational because we can't use the interrupt

	while (1){																									//main program loop	
		updateCurtiss();																						//update the values retrieved from the Curtis
		drive();																								//update the values send to various components
		#ifdef DEBUG_MODE
			printinfo();																						//print info in the terminal, only when debug mode is enabled
		#endif
	}
}

/*DEBUG
*This prints a lot of information about the alley hoop to the serial port, which can be read with a laptop via the USB port, using (for example) Putty. This is disabled by default as writing takes a lot of processing time.
*This shouldn't matter in practice, but might give problems in the future when the CAN BUS is connected. 
*To enable, please define DEBUG_MODE in overhead.h
*/


#ifdef DEBUG_MODE 
void printinfo(){
	Serial.print(analogRead(ai_hmi_steering));
	Serial.print("DM: ");
	Serial.print(drivemode);
	Serial.print(" drive dir: ");
	Serial.print(driving_direction);
	Serial.print(" engine_dv = ");
	Serial.print(engine_dv);
	Serial.print(" brake_dv = ");
	Serial.print(braking_dv);
	Serial.print(" steering_dv = ");
	Serial.print(steering_dv);
	Serial.print(" steering_cv = ");
	Serial.print(steering_cv);
	Serial.print(" steering_ov = ");
	Serial.print(steering_ov);
	Serial.print(" steering_delta = ");
	Serial.println(steering_delta);
}
#endif

/*
*read the Curtis CAN_BUS. because interrupt pin currently isn't working, we just update our variables with the latest message every tick, irregardless if it is new or old.
*/
void updateCurtiss(){
	CAN_Curt.readMsgBuf(&rxId_Curt, &len_Curt, rxBuf_Curt);	// Read data: len = data length, buf = data byte(s)
	if((rxId_Curt & 0x40000000) == 0x40000000)					//Ignore remote requests
		return;	
	//disassemble the message to the values and assign them to the corresponding variable
	low_voltage = rxBuf_Curt[0] + (rxBuf_Curt[1] * 256);
	engine_rpm = rxBuf_Curt[2] + (rxBuf_Curt[3] * 256);
	engine_temp = rxBuf_Curt[4] + (rxBuf_Curt[5] * 256);
	control_temp = rxBuf_Curt[6] + (rxBuf_Curt[7] * 256);
	high_voltage = rxBuf_Curt[8] + (rxBuf_Curt[7] * 256);	
}
