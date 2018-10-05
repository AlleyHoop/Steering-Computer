#include "Drive.h"

///Drive variables
int	drivemode = 0;						//position of the switch key
bool driving_direction = true;			//forward (true) or backwards driving
int32_t	driving_dv = 0;							 
 
int32_t steering_dv = 0;				//desired value of the steering
int32_t steering_delta = 0;				//delta of the steering
int32_t steering_cv = 0;				//current value of the steering
uint8_t steering_ov = 0;				//output value of the steering

int32_t engine_dv = 0;					//desired value of the engine
int32_t engine_delta = 0;				//delta of the engine
int32_t engine_cv = 0;					//current value of the engine
uint8_t engine_ov = 0;					//output value of the engine

int32_t braking_dv = 0;					//desired value of the braking
int32_t braking_delta = 0;				//delta of the braking
int32_t braking_cv = 0;					//current value of the braking
uint8_t braking_ov = 0;					//output value of the braking

uint16_t low_voltage;					//values received from the CURTIS via the can bus
uint16_t engine_rpm;
uint16_t engine_temp;
uint16_t control_temp;
uint16_t high_voltage;


//initialize the driving
void initDrive(){
	digitalWrite(do_drive_enable, HIGH);			//set outputs high, enabling components, don't know why you would want them turned off.
	digitalWrite(do_brake1_enable, HIGH);			//but it's designed into the PCB
	digitalWrite(do_brake2_enable, HIGH);
	digitalWrite(do_steer_r_en, HIGH);
	digitalWrite(do_steer_l_en, HIGH);
	digitalWrite(do_verbinding,HIGH);				//let the board power itself on, because design decisions. 
	mode_select();									//get the currently selected drive mode
}

//determine startup mode, priority top to bottom, with default as idle (do nothing)
void mode_select(){
	if(digitalRead(di_hmi_switchstop)) {				//if joystick
		drivemode=drivemode_joystick;
	} else if (digitalRead(di_hmi_switchremote)){		//if remote
		drivemode=drivemode_remote;
	} else if (digitalRead(di_hmi_switchauto)){			//if automatic
		drivemode=drivemode_auto;
	} else {											//if nothing default to idle
		drivemode=drivemode_idle;						
	}
}

void drive(){
	//get correct driving input parameters from selected source (remote and auto not implemented, thus being interpreted as idle)
	mode_select();
	switch(drivemode){
		case drivemode_joystick:
			mode_joystick();
			break;
		case drivemode_idle:
			mode_idle();		
			break;
		case drivemode_auto:
			mode_idle();		//not yet implemented
			break;
		case drivemode_remote:
			mode_idle();		//not yet implemented
			break;	
	}
	run_steer();				//update the steering system
	run_brake();				//update the braking system
	run_curtis();				//update the Curtis
}

void mode_joystick(){
	digitalWrite(do_hv_relais,HIGH);										//enable the HV circuit
	if(engine_rpm==0)														//only allow switching when the car isn't driving
		driving_direction = digitalRead(di_hmi_switchfr);						//Detect if the car needs to be in forward or reverse mode from the HMI signals
	//read the desired value from the joystick for steering and convert it to 10th of degrees
	steering_dv = (analogRead(ai_hmi_steering)-steering_joystick_offset);
	steering_dv *= steering_wheels_max_deg;
	steering_dv /= steering_joystick_maximum;
	
	//read the desired value from the joystick for the driving and split it up into driving and braking
	driving_dv = analogRead(ai_hmi_gas) - driving_joystick_offset;
	if(abs(driving_dv) < driving_deadzone)			//create a dead zone around default joystick position, desired value must at least be bigger then dead zone value (makes standing still easier
		driving_dv = 0;
	
	engine_dv=0;										//default don't drive or break
	braking_dv=0;
	if(driving_direction){								//if driving forward
		if(driving_dv>0)								//and joystick pushed forward
			engine_dv=driving_dv;						//copy the driving parameter to the engine
		else
			braking_dv=abs(driving_dv);					//else brake (and make value positive)
	} else {											//if driving backwards
		if(driving_dv>0)								//and joystick pushed forward
		braking_dv=driving_dv;							//brake
		else
		engine_dv=abs(driving_dv);						//drive (and make value positive)
	}
}

void mode_auto(){
	//digitalWrite(do_hv_relais,HIGH);
	//not yet implemented
}

void mode_idle(){
	engine_dv=0;								//don't accelerate anymore
	steering_dv=steering_joystick_offset;		//put wheels in default position
	braking_dv=100;								//brake a bit
	//if the car has stopped, disable the engine
	if(engine_rpm==0){
		digitalWrite(do_hv_relais,LOW);			//disengage engine relay
		braking_dv=0;							//stop braking
	}
}

void mode_remote(){
	//digitalWrite(do_hv_relais,HIGH);
	//not (yet) implemented
}

void run_brake(){
	braking_cv = (long(analogRead(ai_brake_pressure) - braking_sensor_offset) * (250 / 1023));				//retrieve the current value of the brake pressure sensor
	braking_delta = braking_dv - braking_cv;
	if(abs(braking_delta)<10)
		braking_delta = 0;
	braking_ov = constrain(braking_dv * braking_kp, 0, 255);
	analogWrite(pwm_brake_pump, braking_dv);															//Write op to brake pump
}

void run_steer(){
	steering_cv = -(long(analogRead(ai_steer_pot)- steering_sensor_offset) * 1000/1023);				//retrieve the current value of the steering potentiometer and convert it to 10th of degrees
	if(abs(steering_dv)<steering_deadzone)																//create a dead zone around default joystick position, desired value must at least be bigger then dead zone value (makes going straight easier
		steering_dv = 0;
	steering_delta = steering_dv - steering_cv;															//difference between desired value and current value
	if(abs(steering_delta)<10)																			//if the two are too close together, don't steer, this to prevent the motor from constantly trying to make small movements which it is not capable of, so it starts skreeching
		steering_delta=0;
	steering_ov = constrain((abs(steering_delta) * steering_kp), 0, 255);								//determine the output PMW value in such a way that the smaller the delta, the slower it goes to smooth out steering
	if (steering_delta < 0) {																			//H bridge settings, let the engine turn the correct way
		analogWrite(pwm_steer_rpwm, steering_ov);														
		analogWrite(pwm_steer_lpwm, 0);
	}
	else if (steering_delta > 0) {
		analogWrite(pwm_steer_rpwm, 0);
		analogWrite(pwm_steer_lpwm, steering_ov);
	}
	else {
		analogWrite(pwm_steer_rpwm, 0);
		analogWrite(pwm_steer_lpwm, 0);
	}	
}

void run_curtis(){
	engine_cv = (engine_rpm * float(100 / 1023));												//the proces value of the engine system is the rpm which we read from the CAN-bus of the curtis
	digitalWrite(do_drive_forward, driving_direction);											//give the curtiss the correct driving direction
	digitalWrite(do_drive_reverse, !driving_direction);
	
	//Set the curtis in the right mode for braking, driving or neutral																		//if we want to brake
		digitalWrite(do_drive_throttleswitch, engine_dv);				//if there is an engine signal, enable listening to the throttle
		digitalWrite(do_drive_brake, braking_dv);						//if there is an braking signal, turn on the braking 

	engine_delta = engine_dv - engine_cv;									//calculate delta
	engine_ov = constrain(engine_delta * engine_kp * 255 / 100, 0, 255);	//calculate output value
	analogWrite(pwm_drive_throttle, engine_ov);								//Write op to throttle signal of curtis
}

