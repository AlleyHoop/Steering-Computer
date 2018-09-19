/*
* Drive.h
*
* Created: 13-9-2018 12:55:37
* Author: Jur Hoekstra
*/


#ifndef __DRIVE_H__
#define __DRIVE_H__

#include "overhead.h"

extern USART Serial;			//links the Serial object so it can be used here to print stuff.

void initDrive();				//initialize, create variables
void mode_select();				//select mode of operation
void drive();					//standard drive funtion, contains stuff that should always happen and then calls current drivemode corresponding function

//per drivemode specific functions, only set parameters
void mode_charge();				//charging
void mode_idle();				//idling
void mode_joystick();			//control via joystick on the dashboard
void mode_remote();				//remote control
void mode_auto();				//automatic driving

void run_steer();					//setup the correct steering outputs
void run_curtis();					//setup the engine outputs
void run_brake();					//setup the correct brake outputs


//Variables
extern int drivemode;				//operational mode of the car
#define  drivemode_idle 0			//Set when the charging plug is not plugged in and the key switch is on (default)
#define  drivemode_joystick 1		//Set when the car is driving via joystick
#define  drivemode_remote 2			//Set when the car is driving remotely
#define  drivemode_auto 3			//Set when the car is driving automatically

///Drive variables
extern int   engine_dv;                 //desired value of the engine
extern int   braking_dv;                //desired value of the brakes
extern int32_t  steering_dv;               //desired value of the steering
extern int32_t  steering_cv;               //process value of the steering
extern uint8_t   steering_op;               //output value of the steering
extern int32_t  steering_delta;               //delta of the brakes


extern int   engine_cv;                 //process value of the engine
extern int   brake_cv;                  //process value of the brakes

extern int   engine_delta;              //delta of the engine (DELTA = SP-PV)
extern long  brake_delta;            //delta of the steering
extern int   engine_op;                 //ouput value of the engine (OP = DELTA * Kp)
extern int   brake_op;                  //output value of the brakes
extern bool  driving_direction;			//High when forward, low when reverse
//Engine parameters
#define engine_joystick_offset 510		//offset for the engine sp from the joystick in %
#define engine_DB 5						//dead band of the engine in %
#define engine_kp 1						//Proportional gain of the engine
//Brake parameters
#define brake_cv_offset 215          //offset for the brake pv in bar
#define brake_DB 5                   //dead band of the brakes in bar
#define brake_kp 1                   //Proportional gain of the brakes

//Steering parameters
#define steering_joystick_offset 507		//value of the steering joystick when not steering
#define steering_joystick_maximum 137		//maximum value of the joystick
#define steering_wheels_max_deg 350;		//maximum angle of the wheel in degrees

#define steering_sensor_offset 512       //offset for the steering actuator (it's resting value)
#define steering_deadzone 50          //minimum steering angle before the car starts steering
#define steering_kp 7                //Proportional gain of the steering
//Curtis CAN
extern int low_voltage;
extern int engine_rpm;
extern int engine_temp;
extern int control_emp;
extern int high_voltage;

#endif //__DRIVEH__
