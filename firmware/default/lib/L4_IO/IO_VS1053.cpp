#include "IO_VS1053.hpp"
#include "lib_VS1053.h"
#include "utilities.h"

#define IO_VS1053_SCLK_MHZ 1
#define IO_VS1053_READY_TO_RECEIVE_DATA 1
#define IO_VS1053_NOT_READY_TO_RECEIVE_DATA 0

static GPIO DREQ(P1_20);
static GPIO SCI_CS(P1_22);
static GPIO SDI_CS(P1_23);

void IO_VS1053_init(void)
{
	ssp0_init(IO_VS1053_SCLK_MHZ);
	ssp0_set_max_clock(IO_VS1053_SCLK_MHZ); // according to data sheet SCLK = CLKI / 7 = 12.288 (p. 48) / 7 = ~1.76 MHz

	DREQ.setAsInput();
	DREQ.enablePullDown();

	SDI_CS.setAsOutput();
	SCI_CS.setAsOutput();

	SDI_CS.setHigh();
	SCI_CS.setHigh();
}


void IO_VS1053_setVolume(void)
{
	char x;
		SCI_CS.setLow();
	x = ssp0_exchange_byte(0x02);
	// printf("char1 : %02x\n", x);
	x = ssp0_exchange_byte(0x00);
	// printf("char2 : %02x\n", x);
	x = ssp0_exchange_byte(0x48);
	// printf("char3 : %02x\n", x);
	x = ssp0_exchange_byte(0x40);
	// printf("char4 : %02x\n", x);
	SCI_CS.setHigh();

	
	SCI_CS.setLow();
	x = ssp0_exchange_byte(0x02);
	// printf("char1 : %02x\n", x);
	x = ssp0_exchange_byte(0x0b);
	// printf("char2 : %02x\n", x);
	x = ssp0_exchange_byte(0x00);
	// printf("char3 : %02x\n", x);
	x = ssp0_exchange_byte(0x00);
	// printf("char4 : %02x\n", x);
	SCI_CS.setHigh();

	delay_ms(50);



	delay_ms(50);

	SCI_CS.setLow();
	x = ssp0_exchange_byte(0x03);
	// printf("char1 : %02x\n", x);
	x = ssp0_exchange_byte(0x00);
	// printf("char2 : %02x\n", x);
	x = ssp0_exchange_byte(0xff);
	// printf("char3 : %02x\n", x);
	x = ssp0_exchange_byte(0xff);
	// printf("char4 : %02x\n", x);
	SCI_CS.setHigh();
}
bool IO_VS1053_writeSDI(char *buffer, uint16_t *offset)
{
	//check if DREQ is high | high = VS1053 ready to receive data
	bool success = false;

	uint16_t localOffset = *offset;
	uint16_t localOffsetMax = (*offset + 32);
	// printf("localOffset %i\n", *offset);
	if (DREQ.read()) 
	{
		SDI_CS.setLow();
		for (uint16_t i=localOffset; i<localOffsetMax; i++)
		{
			// printf("i: %i\n", i);
			// printf("b: %lc\n", buffer[i]);
			// printf("buffer: %c\n", buffer[i]);
			const char x = ssp0_exchange_byte(buffer[i]);
			// printf("returned: %c\n", x);
			(*offset)++; // increment offset value by reference | derference and increment offset
			// printf("offsetINLoop %i\n", *offset);			 
		}
		SDI_CS.setHigh();
		delay_ms(30);
		success = true;
	}
	else
	{
		success = false;
	}

	return success;
}