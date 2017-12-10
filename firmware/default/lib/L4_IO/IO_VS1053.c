#include "IO_VS1053"

#define IO_VS1053_SCLK_MHZ 1

IO_VS1053_control_S IO_VS1053_control;

bool IO_VS1053_init(void)
{
	ssp1_init();
	ssp1_set_max_clock(IO_VS1053_SCLK_MHZ); // according to data sheet SCLK = CLKI / 7 = 12.288 (p. 48) / 7 = ~1.76 MHz

	IO_VS1053_control.DREQ.setAsInput();
	IO_VS1053_control.DREQ.enablePullDown();

	IO_VS1053_control.SCI_CS.setAsOutput();
	IO_VS1053_control.SDI_CS.setAsOutput();
}

bool IO_VS1053_writeSDI(char *buffer)
{
	//check if DREQ is high | high = VS1053 ready to receive data

	if (IO_VS1053_control.DREQ.read())
	{
		puts("VS1053 ready to receive data. Sending Data.");
		
	}
	else
	{
		puts("VS1053 buffer full.");
	}
}