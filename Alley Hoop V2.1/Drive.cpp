#include "Drive.h"

///Drive variables
int	drivemode = 0;
bool  driving_direction = true;

//desired values, these are the user input read from the dashboard or steering computer
                   
int32_t  steering_dv = 0;
int32_t  steering_delta = 0;             //delta of the steering   
int32_t   steering_cv = 0;
uint8_t  steering_op = 0;                //output value of the steering

//current values, these are the current values, measured from the componenents themselves             
int   engine_cv = 0;                  
int   brake_cv = 0;                   
//output values, values send to the componenets
int   engine_op = 0;                  //ouput value of the engine (OP = DELTA * Kp)
int   braking_op = 0;                   //output value of the brakes
//difference between the set values and the process values		        
int   engine_delta = 0;               //delta of the engine (DELTA = SP-PV)
long   brake_delta = 0;                //delta of the brakes

int   engine_dv = 0;
int   braking_dv = 0;
//Curtis CANard
/*
int low_voltage = 0;
int engine_rpm = 0;
int engine_temp = 0;
int control_emp = 0;
int high_voltage = 0;
*/

void initDrive(){
	digitalWrite(do_drive_enable, HIGH);
	digitalWrite(do_brake1_enable, HIGH);
	digitalWrite(do_brake2_enable, HIGH);
	digitalWrite(do_steer_r_en,   HIGH);
	digitalWrite(do_steer_l_en,   HIGH);
	digitalWrite(do_verbinding,HIGH);
	mode_select();
}

//determine startup mode, priority top to bottom, with default as idle (do nothing)
void mode_select(){
	if(digitalRead(di_hmi_switchstop)) {	//if key and joystick
		drivemode=drivemode_joystick;
	} else if (digitalRead(di_hmi_switchremote)){		//if key and remote
		drivemode=drivemode_remote;
	} else if (digitalRead(di_hmi_switchauto)){		//if key and automatic
		drivemode=drivemode_auto;
	} else {
		drivemode=drivemode_idle;							//if nothing, idle
	}
}

void drive(){
	//get correct driving parameters from selected source (remote and auto not implemented, thus being interpreted as idle)
	mode_select();
	switch(drivemode){
		case drivemode_joystick:
			mode_joystick();
			break;
		case drivemode_idle:
			mode_idle();
			break;
		case drivemode_auto:
			mode_idle();
			break;
		case drivemode_remote:
			mode_idle();
			break;	
	}
	run_steer();			//update the steering system
	//run_brake();			//update the braking system
	//run_curtis();			//update the curtiss
}

void mode_joystick(){
	//enable the 12v circuit
	digitalWrite(do_hv_relais,HIGH);
	//Read mode and determine if you need to drive forward or reverse
	driving_direction = digitalRead(di_hmi_switchfr);							//Detect if the car needs to be in forward or reverse mode from the HMI signals
	engine_dv = analogRead(ai_hmi_gas)  - engine_joystick_offset;				//read in the gas signal from the joystick and calculate this to %
	//read the desired value from the joystick for steering and convert it to 10th of degrees
	steering_dv = (analogRead(ai_hmi_steering)-steering_joystick_offset);
	steering_dv *= steering_wheels_max_deg;
	steering_dv /= steering_joystick_maximum;
	
	braking_dv = 0;
	if (engine_dv < 0){
		braking_dv = abs(engine_dv);
		engine_dv = 0;
	}
	engine_dv = engine_dv * 100 / 123;          //calculate the engine setpoint to a range of 0 - 100 %
	braking_dv = braking_dv * 250 / 123;            //calculate the brake setpoint to a range of 0 - 250 bar
}

void mode_auto(){
	digitalWrite(do_hv_relais,HIGH);
	//not yet implemented
}

void mode_idle(){
	//disable componenets
	engine_op=0;
	steering_op=0;
	braking_op=0;
	//if the car has stopped, disable the engine
	if(engine_dv==0){
		digitalWrite(do_hv_relais,LOW);
	}
}

void mode_remote(){
	digitalWrite(do_hv_relais,HIGH);
	//not (yet) implemented
}

void run_brake(){
	brake_cv = (long(analogRead(ai_brake_pressure) - brake_cv_offset) * (250 / 1023));			//the proces value of the braking system is the pressure sensor
	
	///if the value of delta is smaller than the dead zone set to 0 if its larger substratct the dead zone
	if (abs(brake_delta) > brake_DB)
	brake_delta = brake_delta - brake_DB;
	else
	brake_delta = 0;
	// tijdelijk de stand van de joystick als pump aansturing gebruikt (Proportioneelmet stand). De druk wordt hierbij weggelaten
	//  analogWrite(pwm_brake_pump, brake_op);         //Write op to brake pump
	braking_dv =  constrain(braking_dv, 0, 255);
	//brake_op =  constrain(brake_delta * brake_kp  * float(255 /0 250), 0, 255);
	analogWrite(pwm_brake_pump, braking_dv);         //Write op to brake pump
}

void run_steer(){
	//steering_cv = analogRead(ai_steer_pot);
	steering_cv = -(long(analogRead(ai_steer_pot)- steering_sensor_offset) * 1000/1023);				//the proces value of the steering system is the linear potentiometer
	//create a deadzone arount default joystick position, desired value must at least be bigger then deadzone value
	if(abs(steering_dv)<steering_deadzone)
		steering_dv = 0;
	
	//difference between desired value and current value
	steering_delta = steering_dv - steering_cv;
	//if the two are too close together, except when in the deadzone
	if(abs(steering_delta)<10)
		steering_delta=0;
	steering_op = constrain((abs(steering_delta) * steering_kp), 0, 255);
	
	if (steering_delta < 0) {
		Serial.print(" steering left ");
		analogWrite(pwm_steer_rpwm, steering_op);
		analogWrite(pwm_steer_lpwm, 0);
	}
	//if delta is negative steer left
	else if (steering_delta > 0) {
		Serial.print(" steering right");
		analogWrite(pwm_steer_rpwm, 0);
		analogWrite(pwm_steer_lpwm, steering_op);
	}
	//if delta = 0 stay in position
	else {
		Serial.print(" not steering  ");
		analogWrite(pwm_steer_rpwm, 0);
		analogWrite(pwm_steer_lpwm, 0);
	}	
}

void run_curtis(){
	engine_cv = (engine_rpm * float(100 / 1023));												//the proces value of the engine system is the rpm which we read from the CAN-bus of the curtis
	//When the car is standing still determine if the car needs to drive in forward or reverse
	digitalWrite(do_drive_forward,  driving_direction);
	digitalWrite(do_drive_reverse,  !driving_direction);
	//Set the curtis in the right mode for braking, driving or neutral
	///Check if there is a brake stepoint, if yes we will configure the curtis in the following way
	if (braking_dv > brake_DB) {
		engine_dv = 0;                                //the engine setpoint will be set to zero
		digitalWrite(do_drive_throttleswitch, LOW);   //the throttleswitch of the curtis will be turned off (this detects if there is being pressed on the gas pedal if we turn this of it will not read the throttle signal)
		digitalWrite(do_drive_brake,          HIGH);  //the brake switch will be turned on so that the curtis can help braking
	}
	///If there wasnt a brake setpoint check if there is an engine setpoint, if yes we will configure the curtis in the following way
	else if (engine_dv > engine_DB) {
		digitalWrite(do_drive_throttleswitch, HIGH);  //the throttle switch of the engine will be turned on so the curtis can receive a throttle signal
		digitalWrite(do_drive_brake,          LOW);   //the brake switch will be turned off
	}
	///If there is no engine or brake setpoint, we will configure the curtis in the following way
	else {
		digitalWrite(do_drive_throttleswitch, LOW);   //the throttle switch will be turned off so the curtis can't recieve a throttle signal
		digitalWrite(do_drive_brake,          LOW);   //the brake switch will be turned off
	}

	//Calculate delta (delta = SP - PV) for the engine, brakes and steering
	engine_delta = engine_dv - engine_cv;
	brake_delta = braking_dv - brake_cv;


	///if the value of delta is smaller than the dead zone set to 0 if its larger substratct the dead zone
	if (abs(engine_delta) > engine_DB)
		engine_delta = engine_delta - engine_DB;  // wat als engine_delta negatief is???? dan moet DB toch opgeteld worden?
	else
		engine_delta = 0;

	engine_op = constrain(engine_delta * engine_kp * 255 / 100, 0, 255);
	///Send OP to engine
	analogWrite(pwm_drive_throttle, engine_op); //Write op to throttle signal of curtis
	
}

