#include "Drive.h"

	void initDrive(){
	unsigned long time;
	
	///Drive variables
	int   engine_sp = 0;                  //set point of the engine
	int   brake_sp = 0;                   //set point of the brakes
	long  steering_sp = 0;                //set point of the steering
	int   engine_pv = 0;                  //process value of the engine
	int   brake_pv = 0;                   //process value of the brakes
	int   steering_pv = 0;                //process value of the steering
	int   engine_delta = 0;               //delta of the engine (DELTA = SP-PV)
	int   brake_delta = 0;                //delta of the brakes
	long  steering_delta = 0;             //delta of the steering
	int   engine_op = 0;                  //ouput value of the engine (OP = DELTA * Kp)
	int   brake_op = 0;                   //output value of the brakes
	long  steering_op = 0;                //output value of the steering
	bool  mode_forward = false;           //High when the driver asks to drive forward
	bool  mode_reverse = false;           //High when the driver asks to drive reverse
	///Parameters - set these accordingly
	//Engine parameters
	int engine_joystick_offset = 510;     //offset for the engine sp from the joystick in %
	int engine_DB = 5;                  //dead band of the engine in %
	int engine_kp = 1;                  //Proportional gain of the engine
	
	//Brake parameters
	int brake_pv_offset = 215;          //offset for the brake pv in bar
	int brake_DB = 5;                   //dead band of the brakes in bar
	int brake_kp = 1;                   //Proportional gain of the brakes
	
	//Steering parameters
	int steering_joystick_offset = 508; //offset for the steering sp from the joystick in °
	int steering_rasp_offset = 128;     //offset for the steering sp from the Raspberry Pi in °
	int steering_pv_offset = 512;       //offset for the steering pv in °
	int steering_DB = 5;                //dead band of the steering in °
	int steering_kp = 70;                //Proportional gain of the steering
	
	//Curtis CAN
	int low_voltage = 0;
	int engine_rpm = 0;
	int engine_temp = 0;
	int control_emp = 0;
	int high_voltage = 0;
	mode_select();
}

//determine startup mode, priority top to bottom, with default as idle (do nothing)
void mode_select(){
	bool key=digitalRead(di_keysignaal);					//read key signal
	if(digitalRead(di_gnd_lader)){							//check if charging
		drivemode=drivemode_charge;
		terminal.println("drivemode charge");	
	} else if(digitalRead(di_hmi_switchjoystick)&&key) {	//if key and joystick
		drivemode=drivemode_joystick;
		terminal.println("drivemode joystick");
	} else if (digitalRead(di_hmi_switchremote)&&key){		//if key and remote
		drivemode=drivemode_remote;
		terminal.println("drivemode remote");
	} else if (digitalRead(di_hmi_switchauto)&&key){		//if key and automatic
		drivemode=drivemode_auto;
		terminal.println("drivemode auto");
	} else {
		drivemode=drivemode_idle;							//if nothing, idle
		terminal.println("drivemode idle");
	}
}

void drive(){
	//set charging false so we know in shut_down what the last called mode was (charging or driving)
	if(charging&&drivemode!=charging){
		charging = false;
		//Disable charger
		digitalWrite(do_pilotswitch,  LOW); // t.b.v. laadpaal comm.
		//Enable steering-, brake- and engine control system
		digitalWrite(do_drive_enable, HIGH);
		digitalWrite(do_brake1_enable, HIGH);
		digitalWrite(do_brake2_enable, HIGH);
		digitalWrite(do_steer_r_en,   HIGH);
		digitalWrite(do_steer_l_en,   HIGH);
	}
	//Read the can of the curtis
	//checkCAN_curtis();
	
	//get correct driving parameters from selected source (remote and auto not implemented, thus being interpreted as idle)
	switch(drivemode){
		case drivemode_joystick:
			mode_joystick();
			break;
		case drivemode_charge:
			mode_charge();
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

	run();
}

void mode_charge(){
	//Disable engine and steering control system
	if(!charging){
		digitalWrite(do_drive_enable, LOW);
		digitalWrite(do_steer_r_en,   LOW);
		digitalWrite(do_steer_l_en,   LOW);
		digitalWrite(do_brake1_enable, LOW);
		digitalWrite(do_brake2_enable, LOW);
	}
	charging=true;
	//Enable charger
	digitalWrite(do_pilotswitch,  HIGH);	
}

void mode_joystick(){
	//Read setpoint's for chosen mode and determine if you need to drive forward or reverse
	///Check if the car is in joystick mode
	//Detect if the car needs to be in forward or reverse mode from the HMI signals
	mode_forward = digitalRead(di_hmi_switchfr);
	mode_reverse = (!digitalRead(di_hmi_switchfr));
	
	//Calculate setpoint from the HMI in the right unit
	engine_sp   = analogRead(ai_hmi_gas)  - engine_joystick_offset;        //read in the gas signal from the joystick and calculate this to %
	steering_sp = analogRead(ai_hmi_steering) - steering_joystick_offset; //read in the steering signal from the joystick and calcualte this to °
	brake_sp = 0;
	if (engine_sp < 0){
		brake_sp = abs(engine_sp);
		engine_sp = 0;
	}

	// *************************** heb mijn twijvels over de juiste waardes en begrenzingen ****************************************

	engine_sp = engine_sp * 100 / 123;          //calculate the engine setpoint to a range of 0 - 100 %
	brake_sp = brake_sp * 250 / 123;            //calculate the brake setpoint to a range of 0 - 250 bar
	steering_sp = steering_sp * 270L / 246L;      //calculate the steering setpoint to a range of -135 - 135°
	if(steering_sp>40)
		steering_sp = -steering_sp; // tijdelijk gedaan omdat joystick verkeertom werkt
}

void mode_auto(){
		/*
		///Detect if the car is in auto mode
		//Detect if the car needs to be in forward or reverse mode from the Raspberry Pi signals
		else if (mode_auto == true) {
			if (rasp_switchfr == true) {
				mode_forward = rasp_switchfr;
				mode_reverse = (!rasp_switchfr);
		//Calculate setpoint from the Raspberry Pi signals in the right unit
		else if (mode_auto == true) {
			//Read CAN data of the raspberry pi
			checkCAN_rasp();

			//Map the CAN data from 8 bit to 10 bit
			map(rasp_gas, 0, 255, 0, 1023);
			map(rasp_brake, 0, 255, 0, 1023);
			map(rasp_steering, 0, 255, 0, 1023);

			engine_sp = rasp_gas;
			brake_sp = rasp_brake;
			steering_sp = rasp_steering;
		}
		*/
}

void mode_idle(){
	
}

void mode_remote(){
	
}


void run(){
	//Read the proces value of the steering, brakes and engine
	engine_pv = (engine_rpm * float(100 / 1023));                                          //the proces value of the engine system is the rpm wich we read from the CAN-bus of the curtis
	brake_pv = (long(analogRead(ai_brake_pressure) - brake_pv_offset) * (250 / 1023));    //the proces value of the braking system is the pressure sensor
	steering_pv = (long(analogRead(ai_steer_pot)- steering_pv_offset) * 100/1023);    //the proces value of the steering system is the linear potentiometer

	//When the car is standing still determine if the car needs to drive in forward or reverse
	//  if (engine_rpm == 0) {
	///If mode_forward is true enable do_drive_forward and disable do_drive_reverse
	if (mode_forward) {
		digitalWrite(do_drive_forward,  mode_forward);
		digitalWrite(do_drive_reverse,  !mode_forward);
	}
	///If mode_reverse is true enable do_drive_reverse and disable do_drive_forward
	if (mode_reverse) {
		digitalWrite(do_drive_reverse,  mode_reverse);
		digitalWrite(do_drive_forward,  !mode_reverse);
	}
	//    else {
	//      digitalWrite(do_drive_reverse,  LOW);
	//    }
	//  }

	//Set the curtis in the right mode for braking, driving or neutral
	///Check if there is a brake stepoint, if yes we will configure the curtis in the following way
	if (brake_sp > brake_DB) {
		engine_sp = 0;                                //the engine setpoint will be set to zero
		digitalWrite(do_drive_throttleswitch, LOW);   //the throttleswitch of the curtis will be turned off (this detects if there is being pressed on the gas pedal if we turn this of it will not read the throttle signal)
		digitalWrite(do_drive_brake,          HIGH);  //the brake switch will be turned on so that the curtis can help braking
	}
	///If there wasnt a brake setpoint check if there is an engine setpoint, if yes we will configure the curtis in the following way
	else if (engine_sp > engine_DB) {
		digitalWrite(do_drive_throttleswitch, HIGH);  //the throttle switch of the engine will be turned on so the curtis can receive a throttle signal
		digitalWrite(do_drive_brake,          LOW);   //the brake switch will be turned off
	}
	///If there is no engine or brake setpoint, we will configure the curtis in the following way
	else {
		digitalWrite(do_drive_throttleswitch, LOW);   //the throttle switch will be turned off so the curtis can't recieve a throttle signal
		digitalWrite(do_drive_brake,          LOW);   //the brake switch will be turned off
	}

	//Calculate delta (delta = SP - PV) for the engine, brakes and steering
	engine_delta = engine_sp - engine_pv;
	brake_delta = brake_sp - brake_pv;
	steering_delta = steering_sp - steering_pv;

	///if the value of delta is smaller than the dead zone set to 0 if its larger substratct the dead zone
	if (abs(engine_delta) > engine_DB)
		engine_delta = engine_delta - engine_DB;  // wat als engine_delta negatief is???? dan moet DB toch opgeteld worden?
	else
		engine_delta = 0;

	///if the value of delta is smaller than the dead zone set to 0 if its larger substratct the dead zone
	if (abs(brake_delta) > brake_DB)
		brake_delta = brake_delta - brake_DB;
	else
		brake_delta = 0;

	///if the value of delta is smaller than the dead zone set to 0 if it is larger substratct the dead zone in case of positive and add the dead zone in case of a negative delta
	if (abs(steering_delta) > engine_DB) {
		if (steering_delta > 0) {
			steering_delta = steering_delta - steering_DB;
		}
		else {
			steering_delta = steering_delta + steering_DB;
		}
	}
	else {
		steering_delta = 0;
	}

	//Determine the output
	//calculate the op value to a resolution for analog write, 8 bit (0-255)
	engine_op = constrain(engine_delta * engine_kp * 255 / 100, 0, 255);
	brake_op =  constrain(brake_delta * brake_kp  * float(255 / 250), 0, 255);
	steering_op = constrain((abs(steering_delta) * steering_kp) * 255 / 270, 0, 255);

	//Send the OP to the devices
	///Send OP to engine
	analogWrite(pwm_drive_throttle, engine_op); //Write op to throttle signal of curtis
	///Send OP to brakes

	// tijdelijk de stand van de joystick als pump aansturing gebruikt (Proportioneelmet stand). De druk wordt hierbij weggelaten
	//  analogWrite(pwm_brake_pump, brake_op);         //Write op to brake pump
	brake_sp =  constrain(brake_sp, 0, 255);
	analogWrite(pwm_brake_pump, brake_sp);         //Write op to brake pump

	//sturen tijdelijk even uitgezet omdat dit eerst goed getest moet worden.
	//                //Send OP to steering
	//                //if delta is positive steer right
	if (steering_delta > 0) {
		analogWrite(pwm_steer_rpwm, steering_op);
		analogWrite(pwm_steer_lpwm, 0);
	}
	//if delta is negative steer left
	else if (steering_delta < 0) {
		analogWrite(pwm_steer_rpwm, 0);
		analogWrite(pwm_steer_lpwm, steering_op);
	}
	//if delta = 0 stay in position
	else {
		analogWrite(pwm_steer_rpwm, 0);
		analogWrite(pwm_steer_lpwm, 0);
	}
}
