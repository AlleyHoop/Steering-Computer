#include "IOPins.h"

//this function should be called when initializing the program. It writes the correct settings to the registry, allowing the use of analog and digital reading and writing of pins.
void initIO(){
	/*Parts of these pins are used for different functions, don't just attach them to the pins:
	Timer1: 
	
	*/
	Serial.println("Applying IO settings...");
	//////////////////////////////////////
	//Input;Output
	//////////////////////////////////////
	//Designate output pins: Disable the pull up resistor in these pins and making them output by writing a 1 to their respective registry entries. Pins not written to default to input pins.
	//when adding/altering pins, alter them in their respective switch case as well.
	//digitalwrite
	//		  76543210
	DDRA |= 0b11110111;				//pin 22-29
	DDRB |= 0b00000000;				//PB0-3 are reserved for SPI, don't use those
	DDRC |= 0b10011000;				
	DDRD |= 0b00000000;
	DDRE |= 0b00110000;
	DDRF |= 0b00000000;				//analog pins A0-A7
	DDRG |= 0b00000000;
	DDRH |= 0b00110000;
	DDRJ |= 0b00000000;				//only used for Serial3
	//DDRI doesnt exist
	DDRK |= 0b00000000;				// analog pins A8-A15
	DDRL |= 0b00000000;				// digital pins 49-42
	//analogwrite, Not all pins support PWM signals, please consult data sheets before altering.
	
	
	DDRG |= 0b00100000;
	DDRH |= 0b01111000;
	DDRL |= 0b00111000;
	
	//remote control, digital inputs
	//////////////////////////////////////
	//Timers, used for PWM and PulseIn
	//////////////////////////////////////
	//when something is unclear or you want to change some setting, please consult the data sheet first.
	
	//Compare Output Mode
	//determines how the PWN behaves, now it Clears OCxx on compare match. This makes it so that low value OCRxx will result in short pulses, while higher result in longer ones
	//COM0xx0 will make OCxx toggle on compare match, both will set om compare match. Neither will disable the pmw by disconnecting OCxx.
	TCCR0A |= (( 1 << COM0A1) | (1 << COM0B1));			//Timer0, used for PB7, PG5
	//TCCR1A |= (( 1 << COM1A1) | (1 << COM1B1));		//Timer1, used PulseIn command, measuring pulse lenghts. Because we don't use it for PWM it is disabled as such 
	TCCR2A |= (( 1 << COM2A1) | (1 << COM2B1));			//Timer2, used for PB4, PH6
	TCCR3A |= (( 1 << COM3A1) | (1 << COM3B1));			//TImer3, used for PE3, PE4, PE5
	TCCR4A |= (( 1 << COM4A1) | (1 << COM4B1));			//Timer4, used for PH3, PH5, PH6
	TCCR5A |= (( 1 << COM5A1) | (1 << COM5B1));			//Timer5, used for PL3, PL4, PL5
	
	//Waveform Generation Mode
	//note that here OCRxA, OCRxB and OCRxC use the same registry and thus also use the same waveform.
	//the current PWM is Fast PMW, but in this case the PWM mode isn't really important.
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));									//8 bit
	TCCR1A |= ((1 << WGM13) | (1 << WGM12) | (1 << WGM11) | (1 << WGM10));		//16 bit, TOP=OCR1A
	TCCR2A |= ((1 << WGM21) | (1 << WGM20));									//8 bit
	TCCR3A |= ((1 << WGM32) | (1 << WGM30));									//16 bit timer, scaled back to 8 bit. 
	TCCR4A |= ((1 << WGM42) | (1 << WGM40));									//16 bit timer, scaled back to 8 bit. 
	TCCR5A |= ((1 << WGM52) | (1 << WGM50));									//16 bit timer, scaled back to 8 bit. 
	
	//Clock Select
	//currently selected, Internal clock, /8 prescaler. this starts the PWM as well
	//F_CPU=16000000 / 256 / 8 ~= 7812Hz wave
	//again as with the OCRxx, these use the same registry as well and are thus linked
	TCCR0B |= (1 << CS01);
	TCCR1B |= ((1 << CS10) | (1 << CS12));		//no prescaling
	TCCR2B |= (1 << CS21);
	TCCR3B |= (1 << CS31);
	TCCR4B |= (1 << CS41);
	TCCR5B |= (1 << CS51);
	/*
	//initialize counters
	TCNT1=0;
	
	//Timer/Counter Interrupt Mask Register
	TIMSK1 |= (1 << TOIE1);
	
	sei();	//enable global interrupts*/
	//initialize each PWM as 0
	OCR3B=0;	//pin 2
	OCR3C=0;	//pin 3
	OCR0B=0;	//pin 4
	OCR3A=0;	//pin 5
	OCR4A=0;	//pin 6
	OCR4B=0;	//pin 7
	OCR4C=0;	//pin 8
	OCR2B=0;	//pin 9
	OCR2A=0;	//pin 10
	OCR1A=0;	//pin 11
	//OCR1B=0;	//pin 12
	//OCR1C=0;	//pin 13
	OCR5A=0;	//pin 44
	OCR5B=0;	//pin 45
	OCR5C=0;	//pin 46
	
	//////////////////////////////////////
	//Analog Digital Converter, analogread
	//////////////////////////////////////
	
	//Analog Digital Multiplexer Selection Register, voltage reference selection
	//Current setting: AVcc with external capacitor at AREF pin (internal 5v)
	ADMUX |= (1 << REFS0);
	
	// 1/128 prescaler
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	
	//enable the ADC convector
	ADCSRA |= (1 << ADEN);
	Serial.println("IO setting generated");
	return;
}


//writes a value between 0 and 255 to the correct output compare register
//analogWrite(pin number, value it should receive).
void analogWrite(int pin,int val){
	if(val>255)			//if the given value exceeds 255, make it 255
	val=255;
	switch(pin){
		case 2:
		OCR3B=val;
		return;
		case 3:
		OCR3C=val;
		return;
		case 4:
		OCR0B=val;
		return;
		case 5:
		OCR3A=val;
		return;
		case 6:
		OCR4A=val;
		return;
		case 7:
		OCR4B=val;
		return;
		case 8:
		OCR4C=val;
		return;
		case 9:
		OCR2B=val;
		return;
		case 10:
		OCR2A=val;
		return;
		case 11:
//		used for timer1
// 		OCR1A=val;
// 		return;
// 		case 12:
// 		OCR1B=val;
// 		return;
		case 44:
		OCR5C=val;
		return;
		case 45:
		OCR5B=val;
		return;
		case 46:
		OCR5C=val;
		return;
		}
	Serial.println("ERROR: tried to write analog value to unsupported pin");
	return;
}

//writes a digital value to a pin.
void digitalWrite(int pin,bool val){
	if(pin<22||pin>53){					//check if the pin is a digital IO pin and supported as such
		Serial.print("ERROR: tried to write digital value to unsupported digital pin");
		Serial.println(pin);
	}
	else if(pin<30){					//check if pin is in the A register
		pin-=22;						//make pin range 0-7
		if(val) PORTA |= (1<<pin);		//set correct bit in A register is val==true
		else PORTA &= ~(1<<pin);		//unset bit if false
	}
	else if(pin<38){
		pin=(37-pin);
		if(val) PORTC |= (1<<pin);
		else PORTC &= ~(1<<pin);
	}
	else if(pin==38){
		if(val) PORTD |= (1<<PIND7);
		else PORTD &= ~(1<<PIND7);
	}
	else if(pin<42){
		pin=(41-pin);
		if(val) PORTG |= (1<<pin);
		else PORTG &= ~(1<<pin);
	}
	else if(pin<50){
		pin=(49-pin);
		if(val) PORTL |= (1<<pin);
		else PORTL &= ~(1<<pin);
	}
	else if(pin<54){
		pin=(53-pin);
		if(val) PORTB |= (1<<pin);
		else PORTB &= ~(1<<pin);
	}
	return;
}

bool digitalRead(int pin){
	if(pin==4){
		if(PING&(1<<PING5))	return true;
	}
	else if(pin<30){					//check if pin is in the A register
		pin-=22;						//make pinrange 0-7
		if(PINA&(1<<pin))	return true;							//break function
	}
	else if(pin<38){
		pin=37-pin;
		if(PINC&(1<<pin))	return true;							//break function
	}
	else if(pin==38){
		if(PIND&(1<<PIND7))	return true;							//break function
	}
	else if(pin<42){
		pin=41-pin;
		if(PING&(1<<pin))	return true;							//break function
	}
	else if(pin<50){
		pin=49-pin;
		if(PINL&(1<<pin))	return true;							//break function
	}
	else if(pin<54){
		pin=(53-pin);
		if(PINB&(1<<pin))	return true;							//break function
	}
	else{					//pin unsupported
		Serial.print("ERROR: tried to read digital value from unsupported digital pin: ");
		Serial.println(pin);
		return false;
	}
	return false;	
}

int analogRead(int pin){
	//set correct MUX registers, these determine on which pin the ADC should read it's value
	//first the MUXx bits are cleared, then filled to prevent old setting leaking though
	
	ADMUX &=(0b11100000);							//clear the bottom 4 bit
	ADCSRB &=(0b11110111);							//clear the top one bit
	if(pin<8)										//if adc #0-7
	ADMUX |= pin;								//write the full pin to the ADMUX registry
	else if(pin<16){									//if adc #8-15
		ADCSRB |= (1<<3);							//write upper bit to ADCSRB register
		ADMUX |= (pin-8);							//write lower bits to ADMUX registry
	}
	else{
		Serial.print("ERROR: tried to read digital value from unsupported analog pin ");
		Serial.println(pin);
		return 0;
	}
	ADCSRA |= (1<<ADSC);							//start ADC conversion
	while(ADCSRA & (1<<ADSC));						//wait until the ADSC is 0 again and the conversion is done
	return ADC;										//return the byte in ADLAR, the result of the ADC conversion
}
