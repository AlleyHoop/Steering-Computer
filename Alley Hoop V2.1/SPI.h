#ifndef __SPI_H__
#define __SPI_H__

using namespace std;
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>			//massive define lists with registries

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

class SPISettings {
	public:
	SPISettings(uint32_t, uint8_t, uint8_t);
	SPISettings();
	~SPISettings(){}								//deconstructor
	private:
	void init_AlwaysInline(uint32_t, uint8_t, uint8_t);
	uint8_t spcr;
	uint8_t spsr;
	friend class SPIClass;
};

class SPIClass {
	public:
	static void begin();					// Initialize the SPI library
	void beginTransaction(SPISettings);
	uint8_t transfer(uint8_t);			// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
	uint16_t transfer16(uint16_t);
	void transfer(void *buf, size_t);
	void endTransaction(){};				//not used
	static void end();					//Disable the SPI bus
	private:
	static uint8_t initialized;
};

extern SPIClass SPI;

#endif
