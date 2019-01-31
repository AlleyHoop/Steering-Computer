/*
* IOPins.h
* These functions take care op properly reading and writing to the IO pins, both digitally (true/false) and analog.
* Created: 11-9-2018 09:15:29
* Author: Jur Hoekstra
*/

#ifndef __IOPINS_H__
#define __IOPINS_H__

//includes

#include "USART.h"
extern USART Serial;			//links the Serial object so it can be used here to print stuff.


//functions
void initIO();					//initializes the IO pins by writing the correct values to the registry. See IOPins.cpp for more detailed information
void analogWrite(int,int);		//allows writing analog values to pins.
int analogRead(int);			//allows writing digital values to pins
void digitalWrite(int,bool);	//allows reading a analog value from a pin		when using, remove the A from the pin number, it will default to the analog inputs
bool digitalRead(int);			//allows reading a digital value from a pin

/*
Pin name Defines. 
Makes the code easier to read by assigning a descriptive name to each pin, describing  their function, while aslo making remapping the pins easier
*/

// Analog
// While the arduino feels the need to shoehorn these pins as analog only, this is not true and we can (and will) use them as digital pins, as in reality the atmega doesnt care just like a pwm pin can be a normal digital output
#define ai_brake_pressure			A0				//pinF0, pressure sensor of the braking system
#define	do_keepalive				A1				//pinF1, prevents the car from shutting of with the key while driving
#define	DI_Rem_Channel_1			A2				//pinF2, Remote channel 1
#define	DI_Rem_Channel_2			A3				//pinF3, Remote channel 2
#define	DI_Rem_Channel_3			A4				//pinF4, Remote channel 3
#define	DI_Rem_Channel_4			A5				//pinF5, Remote channel 4
#define	DI_Rem_Channel_5			A6				//pinF6, Remote channel 5
// #define								A7				//pinF7,
// #define								A8				//pinK0,
// #define								A9				//pinK1,
// #define								A10				//pinK2,
// #define								A11				//pinK3,
// #define								A12				//pinK4,
#define ai_hmi_engine				A13				//pinK5, engine signal of the joystick
#define ai_hmi_steering				A14				//pinK6, steering signal of the joystick
#define ai_steer_pot				A15				//pinK7, potentiometer of the steering system

//PWM					
//#define 								2				//pinE4, 
//#define 								3				//pinE5,
#define	pwm_drive_throttle			4				//pinG5, throttle signal that will control the engine
//#define 								5				//pinE3
#define pwm_brake_pump				6				//pinH3,pwm signal to control the speed of the braking pump
#define pwm_steer_rpwm				7				//pinH4, pwm signal to control the speed of the engine turning right
#define pwm_steer_lpwm				8				//pinH5, pwm signal to control the speed of the engine turning left 
// #define 								9				//pinH6, 
// #define 								10				//pinB4, 
// #define 								11				//pinB5, 
// #define								12				//pinB6,
// #define Onboard_LED					13				//pinB7, onboard LED connected to this pin and can give unexpected behaviour 

//Communication
// these pins have default mapping to different communication protocols, so when using those the pins will get interference.
// if not using these, these pins can be used as regular pins without problems
// currently only Serial0 (pin 0+1) is used, rendering these pins unusable for other functions
// #define								0				//pinE0, reserved for Serial RX
// #define								1				//pinE1, reserved for Serial TX
// #define								14				//pinJ1, Serial3 TX, 
// #define								15				//pinJ0, Serial3 RX
// #define								16				//pinH1, Serial2 TX
// #define								17				//pinH0, Serial2 RX
// #define								18				//pinD3, Serial1 TX
// #define								19				//pinD2, Serial1 RX
// #define								20				//pinD1, I2C SDA
// #define								21				//pinD0, I2C SCL

/// Digital Pins (22-53)
//#define do_steer_r_en				22				//pinA0,
//#define do_steer_l_en				23				//pinA1, 
#define do_steer_l_en				24				//PinA2, enable for the right direction of the steering motor
#define do_steer_r_en				25				//PinA3, enable for the left direction of the steering motor
//#define								26				//PinA4, 
//#define								27				//PinA5,
#define do_engine_brake				28				//PinA6, enable braking on the curtis
#define do_engine_throttleswitch	29				//PinA7, enable engine power on the curtis
#define do_engine_forward			30				//PinC7, forward enabled for the curtis
#define do_engine_reverse			31				//PinC6, reverse enabled for the curtis
#define do_hv_relay					32				//PinC5, if High, enables the HV relay
#define do_engine_enable			33				//PinC4, enable for the engine control system
#define do_can_curt_CS				34				//PinC3, CS of the curtis canbus
#define do_brake_enable				35				//PinC2, enable for the half bridge for the braking pump
#define	di_hmi_idle					36				//PinC1, mode select of HMI: Idle
// #define								37				//PinC0,
#define di_hmi_switchremote			38				//PinD7, mode select of HMI: Remote
//#define								39				//PinG2,
#define di_hmi_switchjoystick		40				//PinG1, mode select of HMI: joystick
//#define 								41				//PinG0, 
#define	di_hmi_switchauto			42				//PinL7, mode select of HMI: auto
//#define								43				//PinL6,
#define	di_hmi_direction			44				//PinL5, switch to choose between forward and reverse drive of the HMI
// #define								45				//PinL4,
// #define								46				//PinL3,
// #define								47				//PinL2,
#define CAN_1_CS						48				//PinL1, Can 1 CS pin
// #define								49				//PinL0,
// #define								50				//PinB3, reserved SPI MISO, don't use
// #define								51				//PinB2, reserved SPI MOSI, don't use
// #define								52				//PinB1, reserved SPI SCK, don't use
// #define								53				//PinB0, reserved SPI SS, don't use

#endif //__IOPINS_H__
