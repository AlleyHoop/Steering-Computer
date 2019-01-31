#include "Drive.h"

///Drive variables
int	drivemode = 0;						//position of the switch key
bool driving_direction = true;			//forward (true) or backwards driving
int32_t	driving_dv = 0;							 
 
int32_t steering_dv = 0;				//desired value of the steering
int16_t steering_delta = 0;				//delta of the steering
int64_t steering_cv = 0;				//current value of the steering
uint8_t steering_ov = 0;				//output value of the steering

int32_t engine_dv = 0;					//desired value of the engine

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
	digitalWrite(do_engine_enable, HIGH);			//set outputs high, enabling components, don't know why you would want them turned off.
	digitalWrite(do_brake_enable, HIGH);			//but it's designed into the PCB
	digitalWrite(do_steer_l_en,HIGH);
	digitalWrite(do_steer_r_en,HIGH);
	mode_select();									//get the currently selected drive mode
}

//determine startup mode, priority top to bottom, with default as idle (do nothing)
void mode_select(){
	if(digitalRead(di_hmi_switchjoystick)) {				//if joystick
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
	//prevent values past the maximum values being send, just a safety precaution
	if(steering_dv>steering_max_deg)		//if the value exeeds the set maximum value
		steering_dv=steering_max_deg;		//make it it maximum allowed value
	if(steering_dv<-steering_max_deg)
		steering_dv=-steering_max_deg;
	
	run_steer();				//update the steering system
	run_brake();				//update the braking system
	run_curtis();				//update the Curtis
}

void mode_joystick(){
	digitalWrite(34,HIGH);
	digitalWrite(do_hv_relay,HIGH);													//enable the HV circuit
	if(engine_rpm==0)																//only allow switching when the car isn't driving
		driving_direction = digitalRead(di_hmi_direction);							//Detect if the car needs to be in forward or reverse mode from the HMI signals
	
	//steering	
	steering_dv = (analogRead(ai_hmi_steering)-joystick_steer_offset);				//get the offset from the default position by subtracting the offset from the retrieved value
	steering_dv *= steering_max_deg;												//convert this to degrees by multiplying with the maximum degrees
	if(steering_dv>=0)																//divide by the maximum to get the proper value ratio, split up for left and right steering because the joystick has different maximum values for each side
		steering_dv /= joystick_steer_right_maximum;
	else
		steering_dv /= joystick_steer_left_maximum;	

	//gas and brake
	driving_dv = analogRead(ai_hmi_engine) - joystick_drive_offset;					//get the offset from the default position by subtracting the offset from the retrieved value
	driving_dv *= drive_max_PWM;													//multiply this with the maximum PMW signal. When we later divide by the maximum value for the direction, we are left with a pure PMW signal 
	engine_dv=0;																	//default don't drive or break
	braking_dv=0;
	if(driving_direction){															//if driving forward
		if(driving_dv>0)															//and joystick pushed forward
			engine_dv=driving_dv/joystick_engine_maximum;								//copy the driving parameter to the engine
		else
			braking_dv=abs(driving_dv/joystick_brake_maximum);						//else brake (and make value positive)
	} else {																		//if driving backwards
		if(driving_dv>0)															//and joystick pushed forward
		braking_dv=driving_dv/joystick_brake_maximum;								//brake
		else
		engine_dv=abs(driving_dv/joystick_engine_maximum);							//drive (and make value positive)
	}
	braking_dv = constrain(braking_dv,0,255);										//safety precaution so no out of bound values can be parsed									
}

void mode_auto(){
	//digitalWrite(do_hv_relay,HIGH);
	//not yet implemented
}

void mode_idle(){
	engine_dv=0;								//don't accelerate anymore
	steering_dv=0;								//put wheels in default position
	braking_dv=100;								//brake a bit
	//if the car has stopped, disable the engine
	if(engine_rpm==0){
		digitalWrite(do_hv_relay,LOW);			//disengage engine relay
		braking_dv=0;							//stop braking
	}
}

void mode_remote(){
	//digitalWrite(do_hv_relay,HIGH);
	//not (yet) implemented
}

void run_brake(){
	braking_dv = (braking_dv<brake_min ? brake_min : braking_dv);		//if smaller than the minium value to upkeep base pressure, increase to base pressure
	analogWrite(pwm_brake_pump, braking_dv);															//Write op to brake pump
}

void run_steer(){
	steering_cv = (analogRead(ai_steer_pot)-steering_sensor_offset);							//get the offset from the default position by subtracting the offset from the retrieved value
	steering_cv = -steering_cv;																	//because the potentiometer gives it value the other way(left = max, right is minimum), invert this.
	steering_cv *= steering_max_deg;															//convert to degrees
	steering_cv /= steering_sensor_maximum;														//divide by the maximum to get the ratio between 0-1
	
	if(abs(steering_dv)<steering_deadzone)														//create a dead zone around default joystick position, desired value must at least be bigger then dead zone value (makes going straight easier
		steering_dv = 0;
	steering_delta = steering_dv - steering_cv;													//difference between desired value and current value
	if((abs(steering_delta)<10) && (steering_dv!= 0))											//if the two are too close together except the default position, don't steer, this to prevent the motor from constantly trying to make small correcting movements which it is not capable of, so it starts screeching
		steering_delta=0;
	steering_ov = constrain((abs(steering_delta) * steering_kp), 0, 255);						//determine the output PMW value in such a way that the smaller the delta, the slower it goes to smooth out steering
	if(steering_ov<steering_engine_minimum&&steering_ov!=0)
		steering_ov=steering_engine_minimum;
		
	if (steering_delta > 0) {																	//H bridge settings, let the engine turn the correct way
		analogWrite(pwm_steer_rpwm, steering_ov);												//if the delta is smaller than 0, turn to the right by changing the polarity of the H-bridge
		analogWrite(pwm_steer_lpwm, 0);															//
	}
	else if (steering_delta < 0) {
		analogWrite(pwm_steer_rpwm, 0);															//if the delta is larger than 0
		analogWrite(pwm_steer_lpwm, steering_ov);

	}
	else {
		analogWrite(pwm_steer_rpwm, 0);															//if neither, do nothing
		analogWrite(pwm_steer_lpwm, 0);
	}	
}

void run_curtis(){
	digitalWrite(do_engine_forward, driving_direction);											//give the Curtis the correct driving direction
	digitalWrite(do_engine_reverse, !driving_direction);
	
	if(abs(engine_dv)<driving_deadzone)														//create a dead zone around the standing still position, desired value must at least be bigger then dead zone value (makes standing still easier)
		engine_dv = 0;
		engine_dv = constrain(engine_dv, 0, drive_max_PWM);	//constrain the value, preventing an integer overflow if a too high of a value was supplied

			
	//Set the Curtis in the right mode for braking, driving or neutral																		//if we want to brake
		digitalWrite(do_engine_throttleswitch, engine_dv);				//if there is an engine signal, enable listening to the throttle
		digitalWrite(do_engine_brake, braking_dv);						//if there is an braking signal, turn on the braking 

	analogWrite(pwm_drive_throttle, engine_dv);								//Write op to throttle signal of Curtis
}
/*
unsigned long pulseIn(uint8_t channel, uint8_t state){
	//convert the channel to the correct bit in the PF register, and also check if the channel is correct. if not, return 0
	if(channel<1||channel>5){
		Serial.println("tried to read invalid remote channel");
		return 0;
	}
	uint8_t bit = channel++;
	uint8_t stateMask = (state ? bit : 0);
	
	Serial.println("test");
	//convert the timeout values to clock cycles	
	unsigned long maxloops = F_CPU / 16;
	
	unsigned long width = 0;
	// wait for any previous pulse to end 
	while ((PINL & (1<<2)))
	//if the maxloops reaches zero, the timeout is reached and the operation is cancelled
		if (--maxloops == 0)
			return 0;

// wait for the pulse to start
	while ((PINL& (1<<2))==0)
		if (--maxloops == 0)
			return 0;

// wait for the pulse to stop
	while (PINL & (1<<2)) {
		if (++width == maxloops)
		return 0;
		}
	return width;
	

	if (width)
		return ((width * 16 + 16) / ( F_CPU / 1000000L ) );
	else
		return 0;		
}
*/