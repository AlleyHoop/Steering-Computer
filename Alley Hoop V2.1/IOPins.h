/*
* IOPins.h
*	These funcions take care op properly reading and writing to the IO pins, both digitally (true/false) and analog.


* Created: 11-9-2018 09:15:29
* Author: Jur Hoekstra
*/

#ifndef __IOPINS_H__
#define __IOPINS_H__

//includes
#include <avr/io.h>
#include "USART.h"
extern USART Serial;			//links the Serial object so it can be used here to print stuff.


//functions
void initIO();					//initializes the IO pins by writing the correct values to the registry. See IOPins.cpp for more detailed information
void analogWrite(int,int);		//allows writing analog values to pins.
int analogRead(int);			//allows writing digital values to pins
void digitalWrite(int,bool);	//allows reading a analog value from a pin		when using, remove the A from the pinnumber, it will default to the analog inputs
bool digitalRead(int);			//allows reading a digital value from a pin

//defines
/// Analog,
#define ai_steer_pot				A0				//analog pinA00, potentiometer of the steering system
#define ai_brake_pressure			A1				//analog pinA01, pressure sensor of the braking system
#define ai_hmi_gas					A2				//analog pinA02, gas signal of the joystick
#define ai_hmi_brake				A3				//analog pinA03, brake signal of the joystick
#define ai_hmi_steering				A4				//analog pinA04, steering signal of the joystick
//analog pinA05
//analog pinA06
//analog pinA07
//analog pinA08
//analog pinA09
//analog pinA10
//analog pinA11
//analog pinA12
//analog pinA13
//analog pinA14
//analog pinA15

/// PWM
#define pwm_steer_lpwm				2				//PWM pin02, pwm signal to control the speed of the steering motor in the left direction
#define pwm_steer_rpwm				3				//PWM pin03, pwm signal to control the speed if the steering motor in the right direction
#define pwm_brake_pump				7				//PWM pin07, pwm signal to control the speed of the braking pump
#define pwm_drive_throttle			8				//PWM pin08, throttle signal that will control the engine
#define di_remote_gas				9				//PWM pin09, gas signal of the remoteote controller (also brake)
#define di_remote_steering			10				//PWM pin10, steer signal of the remoteote controller
#define di_remote_switchfr			11				//PWM pin11, switch to choose between forward en reverse drving of the remoteote
#define pwm_laadpaal_pulse			12				//PWM pin12,
//PWM pin13	this pin behaves weird, maybe some secondary function is interfering

/// Digital Pins (22-53)
#define do_steer_r_en				22				//digital pin22, enable for the right direction of the steering motor
#define do_steer_l_en				23				//digital pin23, enable for the left direction of the steering motor
#define do_brake1_enable			24				//digital pin24, enable for the braking pump
#define do_brake2_enable			25				//digital pin25, enable for the braking pump
#define do_drive_enable				26				//digital pin26, enable for the engine control system
#define do_drive_forward			27				//digital pin27, forward enbale for the engine control system
#define do_drive_reverse			28				//digital pin28, reverse enable for the engine control system
#define do_drive_brake				29				//digital pin29, brake signal for the engine control sytem
#define do_drive_throttleswitch		30				//digital pin30, signal that needs to be high for the trottle to work of hte engine control system
#define di_keysignaal				31				//digital pin31, input that lets the system know it is on because of the key
#define di_gnd_lader				32				//digital pin32, input that lets the system know it is on because of the charger
#define do_hv_relais				33				//digital pin33, if High HV-relais will turn on
#define do_verbinding				34				//digital pin34, if High the system will keep itself on
#define do_pilotswitch				35				//digital pin35, output to speak with the charging system
//digital pin36
//digital pin37
//digital pin38
#define di_hmi_switchjoystick		39				//digital pin39, mode select of HMI: joystick
#define di_hmi_switchremote			40				//digital pin40, mode select of HMI: remoteote
#define di_hmi_switchauto			41				//digital pin41, mode select of HMI: auto
#define di_hmi_switchfr				42				//digital pin42, switch to choose between forward and reverse drive of the HMI
#define di_hmi_switchstop			43				//digital pin43, switch to make a safe stop of the HMI
//digital pin44
//digital pin45
//digital pin46
//digital pin47
//digital pin48
//digital pin49
//digital pin50
//digital pin53
//digital pin54
//digital pin55

#endif //__IOPINS_H__
