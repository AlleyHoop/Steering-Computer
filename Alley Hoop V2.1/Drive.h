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
void mode_charge();					//charging
void mode_idle();					//idling
void mode_joystick();				//control via joystick on the dashboard
void mode_remote();					//remote control
void mode_auto();					//automatic driving

void run_steer();					//setup the correct steering outputs
void run_curtis();					//setup the engine outputs
void run_brake();					//setup the correct brake outputs


//Variables
extern int drivemode;				//operational mode of the car
#define drivemode_idle 0			//Set when the charging plug is not plugged in and the key switch is on (default)
#define drivemode_joystick 1		//Set when the car is driving via joystick
#define drivemode_remote 2			//Set when the car is driving remotely
#define drivemode_auto 3			//Set when the car is driving automatically

///Drive variables
extern int32_t driving_dv;			//desired value before split into engine_dv and braking_dv

extern int32_t steering_dv;			//desired value of the steering
extern int64_t steering_cv;			//process value of the steering
extern int16_t steering_delta;		//delta of the steering, the difference of the desired value and the current value
extern uint8_t steering_ov;			//output value of the steering


extern int32_t engine_dv;			//desired value of the engine

extern int32_t braking_dv;			//desired value of the braking
extern int32_t braking_cv;			//process value of the braking
extern uint8_t braking_ov;			//output value of the braking
extern int32_t braking_delta;		//delta of the braking

extern bool driving_direction;		//High when forward, low when reverse

//driving parameters
#define braking_sensor_offset 215			//offset for the brake pv in bar
#define braking_kp 3						//Proportional gain of the braking

//joystick parameters
#define joystick_steer_offset 509			//default value of the steering joystick, when not steering x-axle
#define joystick_drive_offset 510			//default value of the steering joystick, when not driving, y-axle

#define joystick_steer_left_maximum 127		//maximum offset of the joystick from the default value, joystick_steer_offset when steering maximum left
#define joystick_steer_right_maximum 140	//maximum offset of the joystick from the default value, joystick_steer_offset when steering maximum right
#define joystick_engine_maximum 126			//maximum offset of the joystick from the default value, joystick_steer_offset when maximum speed
#define joystick_brake_maximum 128			//maximum offset of the joystick from the default value, joystick_steer_offset when full braking



//steering marameters
#define steering_max_deg 270				//maximum angle of the wheels in 10ths of degrees, for reading/debugging purposes only, this value is completely arbitrary and only a ratio. If you want this accurate, measure degrees at maximum outline. Else completely arbitrary. To change the actual maximum angle, change the max value of the potentiometer
#define steering_sensor_offset 512			//default value of the steering sensor, when driving straight
#define steering_sensor_maximum 300			//maximum offset of the steering sensor, measured from the default value, steering_sensor_offset
#define steering_deadzone 20				//minimum steering angle before the car starts steering, in 10th of degrees
#define steering_kp 7						//Proportional gain of the steering

//driving parameters
#define drive_max_PWM 255					//because we have an 8 bit PMW signal to the engine
#define driving_deadzone 20					//minimum engine/brake angle before the car starts reacting, in 10th of degrees

//Curtis CAN
extern uint16_t low_voltage;
extern uint16_t engine_rpm;
extern uint16_t engine_temp;
extern uint16_t control_temp;
extern uint16_t high_voltage;

#endif //__DRIVEH__
