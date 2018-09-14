/* 
* Drive.h
*
* Created: 13-9-2018 12:55:37
* Author: Jur Hoekstra
*/


#ifndef __DRIVE_H__
#define __DRIVE_H__

#include "overhead.h"

extern USART terminal;			//links the terminal object so it can be used here to print stuff.

void initDrive();				//initialize, create variables
void mode_select();				//select mode of operation
void drive();					//standard drive funtion, contains stuff that should always happen and then calls current drivemode corresponding function
//per drivemode specific functions, only set parameters
void mode_charge();				//charging
void mode_idle();				//idling
void mode_joystick();			//control via joystick on the dashboard
void mode_remote();				//remote control
void mode_auto();				//automatic driving
void run();						//send the proper parameters to each component


//Variables
extern int drivemode;						//operational mode of the car
	#define  drivemode_charge 0x01			//Set when the car is charging
	#define  drivemode_idle 0x02			//Set when the charging plug is not plugged in and the key switch is on (default)
	#define  drivemode_joystick 0x03		//Set when the car is driving via joystick
	#define  drivemode_remote 0x04			//Set when the car is driving remotely
	#define  drivemode_auto 0x05			//Set when the car is driving automatically


extern bool charging;			//High when last mode was charging
extern unsigned long time;
///Drive variables
extern int   engine_sp;                 //set point of the engine
extern int   brake_sp;                  //set point of the brakes
extern long  steering_sp;               //set point of the steering
extern int   engine_pv;                 //process value of the engine
extern int   brake_pv;                  //process value of the brakes
extern int   steering_pv;               //process value of the steering
extern int   engine_delta;              //delta of the engine (DELTA = SP-PV)
extern int   brake_delta;               //delta of the brakes
extern long  steering_delta;            //delta of the steering
extern int   engine_op;                 //ouput value of the engine (OP = DELTA * Kp)
extern int   brake_op;                  //output value of the brakes
extern long  steering_op;               //output value of the steering
extern bool  mode_forward;				//High when the driver asks to drive forward
extern bool  mode_reverse;			    //High when the driver asks to drive reverse
//Engine parameters
extern int engine_joystick_offset;     //offset for the engine sp from the joystick in %
extern int engine_DB;                  //dead band of the engine in %
extern int engine_kp;                  //Proportional gain of the engine
//Brake parameters
extern int brake_pv_offset;          //offset for the brake pv in bar
extern int brake_DB;                   //dead band of the brakes in bar
extern int brake_kp;                   //Proportional gain of the brakes
//Steering parameters
extern int steering_joystick_offset; //offset for the steering sp from the joystick in °
extern int steering_rasp_offset;     //offset for the steering sp from the Raspberry Pi in °
extern int steering_pv_offset;       //offset for the steering pv in °
extern int steering_DB;                //dead band of the steering in °*/
extern int steering_kp;                //Proportional gain of the steering
//Curtis CAN
extern int low_voltage;
extern int engine_rpm;
extern int engine_temp;
extern int control_emp;
extern int high_voltage;

#endif //__DRIVEH__
