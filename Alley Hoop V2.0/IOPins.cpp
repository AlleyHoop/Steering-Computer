#include "IOPins.h"
/*

//this funcion should be called when initializng the program. It writes the correct settings to the registry, allowing the use of analog and digital reading and writing of pins.
void initIO(){
	terminal.println("Applying IO settings...");
	//////////////////////////////////////
	//Read/Write pins
	//////////////////////////////////////
	//Disable the pullup resistor in these pins, making them output by writing a 1 to their respective registry entries. Pins not written to default to input pins.
	//when adding/altering pins, alter them in the respective switch case as well. 
	//digitalwrite
	
	DDRB |= 0b00110000;
	DDRD |= 0b00000000;	
	
	*-*
	DDRA |= 0b11111111;
	DDRB |= 0b00001111;
	DDRC |= 0b11111111;
	DDRD |= 0b10000000;
	DDRG |= 0b00000111;
	DDRL |= 0b11111111;
	***
	//analogwrite
	//Not all pins support PWM signals, please consult datasheets before altering.
	DDRB |= 0b00001110;
	DDRD |= 0b01101000;

*-*
	DDRB |= 0b11110000;
	DDRE |= 0b00111000;
	DDRG |= 0b00100000;
	DDRH |= 0b01111000;
	DDRL |= 0b00111000;
***	
	
	//////////////////////////////////////
	//PMW, analogWrite
	//////////////////////////////////////
	//when something is unclear or you want to change some setting, please consult the datasheet first.
	
	*-*Compare output mode
	*determines how the PWN behaves, now it Clears OCxx on compare match. This makes it so that low value OCRxx will result in short pulses, while higher result in longer ones
	*COM0xx0 will make OCxx toggle on compare match, both will set om compare match. Neither will disable the pmw by disconnecting OCxx. ***
	TCCR0A |= (( 1 << COM0A1) | (1 << COM0B1));
	TCCR1A |= (( 1 << COM1A1) | (1 << COM1B1));
	TCCR2A |= (( 1 << COM2A1) | (1 << COM2B1));
	
	*-*Waveform Generation
	*note that here OCRxA and OCRxB use the same registry and thus also use the same waveform.
	*the current PWM is Fast PMW, but in this case the PWM mode isn't really important.***
	TCCR0A |= ((1 << WGM01) | (1 << WGM00));
	TCCR1A |= ((1 << WGM12) | (1 << WGM10));		//16 bit timer, scaled back to 8 bit. (TCCR0x and TCCR2x are 8 bit already)
	TCCR2A |= ((1 << WGM21) | (1 << WGM20));
	
	*-*clock Select
	currently selected, Internal clock, /8 prescaler. this starts the PWM as well
	F_CPU=16000000 / 256 / 8 ~= 7812Hz wave
	again as with the OCRxx, these use the same registry as well and are thus linked***
	TCCR0B |= (1 << CS01);
	TCCR1B |= (1 << CS11);
	TCCR2B |= (1 << CS21);
	
	*-* initialize each pwm as 0***
	OCR2B=0;	//pin 3
	OCR0B=0;	//pin 5
	OCR0A=0;	//pin 6
	OCR1A=0;	//pin 9
	OCR1B=0;	//pin 10
	OCR2A=0;	//pin 11
	
	//////////////////////////////////////
	//Analog Digital Converter, analogread
	//////////////////////////////////////
	
	*-*Analog Digital Multiplexer Selection Register, voltage reference selection
	*Current setting: AVcc with external capacitor at AREF pin (internal 5v)	***
	ADMUX |= (1 << REFS0);							
	
	// 1/128 prescaler
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
	
	//enable the ADC convector, and disable GPIO functionality on the ADC pins
	ADCSRA |= (1 << ADEN);
	terminal.println("IO setting generated");		
	return;
}


//writes a value between 0 and 255 to the correct output compare register
//analogWrite(pin numer, value it should receive).
void analogWrite(int pin,int val){
	if(val>255)			//if the given value exeeds 255, make it 255
		val=255;
	switch(pin){
		case 3:
			OCR2B=val;
			return;
		case 5:
			OCR0B=val;
			return;
		case 6:
			OCR0A=val;
			return;
		case 9:
			OCR1A=val;
			return;
		case 10:
			OCR1B=val;
			return;
		case 11:
			OCR2A=val;
			return;
	}
	terminal.println("ERROR: tried to write analog value to unsupported pin");
	return;
	*-*
	if(val>255)			//if the given value exeeds 255, make it 255
		val=255;
	switch(pin){
		case 2:
			OCR3B=val
			return;
		case 3:
			OCR3C=val;
			return;
		case 4:
			OCR0B=val
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
			OCR1A=val;
			return;
		case 12:
			OCR1B=val;
			return;
		case 13:
			OCR0A=val;
			return;
	}
	terminal.println("ERROR: tried to write analog value to unsupported pin");
	return;***
}

//writes a digital value to a pin.
void digitalWrite(int pin,bool val){
	
	if(~DDRB&&(16))
		terminal.println("out");
	if(pin==12&&(~DDRB&&(1<<PINB4))){			//if pin is pin 12
		if(val)							//if true
			PORTB |= (1 << PINB4);		//OR compare PORTB with 00001000, making our desired bit true while leaving the rest intact
		else							//else, so false
			PORTB &= ~(1 <<PINB4);		//AND compare PORTB 11110111, making our desired bit false, while leaving the rest intact
		return;
	}
	if(pin==13&&(~DDRB&&(32))){
		if(val)
			PORTB |= (1<<PINB5);
		else
			PORTB &= ~(1 <<PINB5);
		return;		
	}
	terminal.print("ERROR: tried to write digital value to unsupported digital pin ");
	terminal.println(pin);
	return;
	*-*
	if(pin<22||pin>53){					//check if the pin is a digital IO pin
		terminal.print("ERROR: tried to write digital value to unsupported digital pin");
		terminal.println(pin);
	}
	else if(pin<30){					//check if pin is in the A register
		pin-=22;						//make pinrange 0-7
		if(val) PORTA |= (1<<pin);		//set correct bit in A register is val==true
		else PORTA &= ~(1<<pin);		//unset bit if false				
	}
	else if(pin<38){
		pin=37-2*(37-pin);
		if(val) PORTC |= (1<<pin);
		else PORTC &= ~(1<<pin);
	}
	else if(pin){
		if(val) PORTD |= (1<<PIND7);
		else PORTD &= ~(1<<PIND7);
	}
	else if(pin<42){
		pin=41-2(40-pin);
		if(val) PORTG |= (1<<pin);
		else PORTG &= ~(1<<pin);
	}
	else if(pin<50){
		pin=54-2(54-pin);
		if(val) PORTL |= (1<<pin);
		else PORTL &= ~(1<<pin);
	}
	else if(pin<54){
		pin=54-2(54-pin);
		if(val) PORTB |= (1<<pin);
		else PORTB &= ~(1<<pin);
	}
	return;
	***
}

bool digitalRead(int pin){
	pin-=7;				//make range start at 0	
	switch(pin){
		case 7:
			if(PIND & (1<<PIND7))			//compare our desired bit with 01000000, thus checking only pinD7
				return true;
			return false;
	}
	terminal.println("ERROR: tried to read digital value from unsupported pin");
	return false;
	
	
	///
	
	*-*
	if(pin<22||pin>53){					//check if the pin is a digital IO pin
		terminal.print("ERROR: tried to read digital value from unsupported digital pin");
		terminal.println(pin);
	}
	else if(pin<30){					//check if pin is in the A register
		pin-=22;						//make pinrange 0-7
		if(~DDRA&&(1<<pin))	return true;							//break function
	}
	else if(pin<38){
		pin=37-2*(37-pin);
		if(~DDRC&&(1<<pin))	return true;							//break function
	}
	else if(pin){
		if(~DDRA&&(1<<PORTD7))	return true;							//break function
	}
	else if(pin<42){
		pin=41-2(40-pin);
		if(~DDRG&&(1<<pin))	return true;							//break function
	}
	else if(pin<50){
		pin=54-2(54-pin);
		if(~DDRL&&(1<<pin))	return true;							//break function
	}
	else if(pin<54){
		pin=54-2(54-pin);
		if(~DDRB&&(1<<pin))	return true;							//break function
	}
	return false;
	***	
}

int analogRead(int pin){
	*-*set correct MUX registers, these determine on which pin the ADC should read it's value
	*	first the MUXx bits are cleared, then filled to prevent old setting leaking though
	***
	ADMUX &=(0b11100000);							//clear the bottom 4 bit
	ADCSRB &=(0b11110111);							//clear the top one bit
	if(pin>8)										//if adc #0-7
		ADMUX |= pin;								//write the full pin to the ADMUX registry
	else if(pin>16){									//if adc #8-15
		ADCSRB |= (1<<3);							//write upper bit to ADCSRB register
		ADMUX |= (pin-8);							//write lower bits to ADMUX registry
	}
	else{
		terminal.print("ERROR: tried to read digital value from unsupported analog pin");
		terminal.println(pin);
		return 0;
	}
	ADCSRA |= (1<<ADSC);							//start ADC conversion
	while(ADCSRA & (1<<ADSC));						//wait until the ADSC is 0 again and the conversion is done			
	return ADC;										//return the byte in ADLAR, the result of the ADC conversion
}
*/