#include "IO_VS1053.hpp"
#include "lib_VS1053.h"
#include "utilities.h"
#include <locale.h>
#include "printf_lib.h"


#define IO_VS1053_INIT_SCLK_MHZ 1
#define IO_VS1053_READY_TO_RECEIVE_DATA 1
#define IO_VS1053_NOT_READY_TO_RECEIVE_DATA 0

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

	setlocale(LC_ALL, "");
}


void IO_VS1053_setVolume(void)
{
	// char x;
	// 	SCI_CS.setLow();
	// x = ssp0_exchange_byte(0x02);
	// // printf("char1 : %02x\n", x);
	// x = ssp0_exchange_byte(0x00);
	// // printf("char2 : %02x\n", x);
	// x = ssp0_exchange_byte(0x48);
	// // printf("char3 : %02x\n", x);
	// x = ssp0_exchange_byte(0x40);
	// // printf("char4 : %02x\n", x);
	// SCI_CS.setHigh();

	
	// // SCI_CS.setLow();
	// // x = ssp0_exchange_byte(0x02);
	// // // printf("char1 : %02x\n", x);
	// // x = ssp0_exchange_byte(0x0b);
	// // // printf("char2 : %02x\n", x);
	// // x = ssp0_exchange_byte(0x00);
	// // // printf("char3 : %02x\n", x);
	// // x = ssp0_exchange_byte(0x00);
	// // // printf("char4 : %02x\n", x);
	// // SCI_CS.setHigh();





	// SCI_CS.setLow();
	// x = ssp0_exchange_byte(0x03);
	// // printf("char1 : %02x\n", x);
	// x = ssp0_exchange_byte(0x0b);
	// // printf("char2 : %02x\n", x);
	// x = ssp0_exchange_byte(0xff);
	// // printf("char3 : %02x\n", x);
	// x = ssp0_exchange_byte(0xff);
	// // printf("char4 : %02x\n", x);
	// SCI_CS.setHigh();
}

void IO_VS1053_setVS1053Clk(void)
{
	SCI_CS.setLow();
	ssp0_exchange_byte(0x02);
	ssp0_exchange_byte(LIB_VS1053_REG_ADDR_SCI_3);
	ssp0_exchange_byte(0x80);
	ssp0_exchange_byte(0x00);
	SCI_CS.setHigh();
	delay_ms(1);
	SCI_CS.setLow();
	ssp0_exchange_byte(0x03);
	ssp0_exchange_byte(LIB_VS1053_REG_ADDR_SCI_3);
	ssp0_exchange_byte(0xff);
	ssp0_exchange_byte(0xff);
	SCI_CS.setHigh();
	delay_ms(1);

	ssp0_set_max_clock(5);

}


bool IO_VS1053_writeSDI(char *buffer, uint16_t *offset)
{
	//check if DREQ is high | high = VS1053 ready to receive data
	bool success = false;

	uint16_t localOffset = *offset;				// 0 // 33
	uint16_t localOffsetMax = (*offset + 32);  // 32 // 36
	// printf("localOffset %i\n", *offset);
	if (DREQ.read()) 
	{
		portENTER_CRITICAL();
		SDI_CS.setLow();
		for (uint16_t i=localOffset; i<localOffsetMax; i++)
		{
			// printf("i: %i\n", i);
			// printf("b: %lc\n", buffer[i]);
			
			(void)ssp0_exchange_byte(buffer[i]);
			// printf("returned: %c\n", x);
			// printf("o: %i\n", *offset);
			// printf("%lc", (buffer[i]));
			(*offset)++; // increment offset value by reference | derference and increment offset
			// printf("o %i\n", *offset);			 
			//1,2,3,4,5...,33
			//33, 34 , 35, 36, ..., 68
		}
		SDI_CS.setHigh();
		portEXIT_CRITICAL();
		success = true;
	}
	else
	{
		success = false;
	}
	
	return success;
}

void IO_VS1053_SDI_CS(bool status)
{
	if (status)
	{
		SDI_CS.setHigh();
	}
	else
	{
		SDI_CS.setLow();
	}
}