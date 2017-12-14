#include "IO_VS1053.hpp"
#include "lib_VS1053.h"
#include "utilities.h"
#include <locale.h>
#include "printf_lib.h"


#define IO_VS1053_INIT_SCLK_MHZ 1
#define IO_VS1053_READY_TO_RECEIVE_DATA 1
#define IO_VS1053_VOLUME_SILENCE 150.0f /// 75dB 
#define IO_VS1053_VOLUME_OFFSET	 15.0f  // 5db | MAX FOR SPEAKERS BEING USES!

#define IO_VS1053_GET_HIGH_BYTE(var) (var >> 8) & 0xFF
#define IO_VS1053_GET_LOW_BYTE(var)  (var & 0xFF)
#define IO_VS1053_VOLUME_SLOPE (IO_VS1053_VOLUME_SILENCE / 100.0f)
						
static GPIO DREQ(P1_20);
static GPIO SCI_CS(P1_22);
static GPIO SDI_CS(P1_23);
static GPIO RESET(P1_29);

void IO_VS1053_init(void)
{
	ssp0_init(IO_VS1053_INIT_SCLK_MHZ);
	ssp0_set_max_clock(IO_VS1053_INIT_SCLK_MHZ); // according to data sheet SCLK = CLKI / 7 = 12.288 (p. 48) / 7 = ~1.76 MHz

	DREQ.setAsInput();
	DREQ.enablePullDown();

	SDI_CS.setAsOutput();
	SCI_CS.setAsOutput();
	RESET.setAsOutput();

	SDI_CS.setHigh();
	SCI_CS.setHigh();

	RESET.setLow();
	delay_ms(10);
	RESET.setHigh();
	delay_ms(50);

	// setlocale(LC_ALL, "");
}


void IO_VS1053_setVolume(uint16_t percentage)
{
	if ((percentage > 95) ||  (percentage > 100))
	{
		percentage = 100;
	}
	else if(percentage < 0)
	{
		percentage = 0;
	}


	// by defintion, silence = 65278, max loudness = 0
	// y = mx + b
	// y = -65278/100x + 65278
	// y = -652.78x + 65278, where x = percentage


	// my speaker max: 5db or 0.5W or 0x0a0a
	// formula: 5db/0.5 = 10 = 0x0a

	// y = mx + b
	// y = -150/100x + 150
	// y = -1.5x + 150, where x = percentage
	printf("percentage: %i\n", percentage);
	uint16_t volume = ((IO_VS1053_VOLUME_SILENCE - (IO_VS1053_VOLUME_SLOPE * (float)percentage)) + IO_VS1053_VOLUME_OFFSET);
	printf("volume: %02x\n", volume);
	// taskDISABLE_INTERRUPTS();
	if (percentage != 0)
	{
	SCI_CS.setLow();
	ssp0_exchange_byte(LIB_VS1053_SPI_WRITE_CMD);
	ssp0_exchange_byte(LIB_VS1053_REG_ADDR_SCI_11);
	ssp0_exchange_byte(volume); // left channel
	ssp0_exchange_byte(volume); // right channel
	SCI_CS.setHigh();
	}
	// taskENABLE_INTERRUPTS();
	// delay_ms(1);
	// SCI_CS.setLow();
	// ssp0_exchange_byte(LIB_VS1053_SPI_READ_CMD);
	// ssp0_exchange_byte(LIB_VS1053_REG_ADDR_SCI_11);
	// ssp0_exchange_byte(0xff);
	// ssp0_exchange_byte(0xff);
	// SCI_CS.setHigh();
	// delay_ms(1);
}

void IO_VS1053_setVS1053Clk(void)
{
	SCI_CS.setLow();
	ssp0_exchange_byte(LIB_VS1053_SPI_WRITE_CMD);
	ssp0_exchange_byte(LIB_VS1053_REG_ADDR_SCI_3);
	ssp0_exchange_byte(0x80);
	ssp0_exchange_byte(0x00);
	SCI_CS.setHigh();
	delay_ms(1);
	SCI_CS.setLow();
	ssp0_exchange_byte(LIB_VS1053_SPI_READ_CMD);
	ssp0_exchange_byte(LIB_VS1053_REG_ADDR_SCI_3);
	ssp0_exchange_byte(0xff);
	ssp0_exchange_byte(0xff);
	SCI_CS.setHigh();
	delay_ms(1);

	// adjust SPI clock to compensate for shield's new internal clock setting 
	ssp0_set_max_clock(5);
}

bool IO_VS1053_writeSDI(char *buffer, uint16_t *offset)
{
	//check if DREQ is high | high = VS1053 ready to receive data
	bool success = false;

	uint16_t localOffset = *offset;				
	uint16_t localOffsetMax = (*offset + 32); 

	if (DREQ.read() == IO_VS1053_READY_TO_RECEIVE_DATA) 
	{
		taskENTER_CRITICAL();
		SDI_CS.setLow();
		for (uint16_t i=localOffset; i<localOffsetMax; i++)
		{
			(void)ssp0_exchange_byte(buffer[i]);
			(*offset)++; // increment offset value by reference | derference and increment offset
		}
		SDI_CS.setHigh();
		taskEXIT_CRITICAL();
		success = true;
	}
	else
	{
		success = false;
	}
	
	return success;
}