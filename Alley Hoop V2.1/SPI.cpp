#include "SPI.h"

SPIClass SPI;

uint8_t SPIClass::initialized = 0;

SPISettings::SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode){
	init_AlwaysInline(clock, bitOrder, dataMode);
}

SPISettings::SPISettings(){
	init_AlwaysInline(4000000, 1, 0x00);
}

void SPISettings::init_AlwaysInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode){
	uint8_t clockDiv =1;
	// Pack into the SPISettings class
	spcr = (1 << SPE) | (1 << MSTR) | ((bitOrder == 0) ? (1 << DORD) : 0) |
	(dataMode & SPI_MODE_MASK) | ((clockDiv >> 1) & SPI_CLOCK_MASK);
	spsr = clockDiv & SPI_2XCLOCK_MASK;
}

void SPIClass::begin(){
	cli(); // Protect from a scheduler and prevent transactionBegin
	if (!initialized) {
		PORTD |= (1<<PIND0);		//SS pin to high
		DDRB |= 0b00000001;			//SS pin to output so it is the SPI master
		DDRB |= 0b00000110;			//MOSI and SCK to output, must happen afterwards,
		SPCR |= (1 << MSTR);
		SPCR |= (1 << SPE);
	}
	initialized++; // reference count;
}

void SPIClass::end() {
	cli();							// Protect from a scheduler and prevent transactionBegin
	if (initialized)					// Decrease the reference counter
		initialized--;
	if (!initialized)				
		SPCR &= ~(1 << SPE);
}

void SPIClass::beginTransaction(SPISettings settings) {
	SPCR = settings.spcr;
	SPSR = settings.spsr;
}

uint8_t SPIClass::transfer(uint8_t data) {
SPDR = data;
while (!(SPSR & (1 << SPIF))) ; // wait
return SPDR;
}

uint16_t SPIClass::transfer16(uint16_t data) {
	union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } in, out;
	in.val = data;
	if (!(SPCR & (1 << DORD))) {
		SPDR = in.msb;
		while (!(SPSR & (1 << SPIF)));
			out.msb = SPDR;
			SPDR = in.lsb;
			while (!(SPSR & (1 << SPIF))) ;
			out.lsb = SPDR;
		} else {
			SPDR = in.lsb;
			while (!(SPSR & (1 << SPIF))) ;
		out.lsb = SPDR;
		SPDR = in.msb;
		while (!(SPSR & (1 << SPIF))) ;
		out.msb = SPDR;
	}
return out.val;
}

 void SPIClass::transfer(void *buf, size_t count) {
	if (count == 0) return;
	uint8_t *p = (uint8_t *)buf;
	SPDR = *p;
	while (--count > 0) {
		uint8_t out = *(p + 1);
		while (!(SPSR & (1 << SPIF))) ;
		uint8_t in = SPDR;
		SPDR = out;
		*p++ = in;
	}
	while (!(SPSR & (1 << SPIF))) ;
	*p = SPDR;
}
