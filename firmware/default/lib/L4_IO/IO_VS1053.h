#ifndef IO_VS1053_H
#define IO_VS1053_H

bool IO_VS1053_init(void);

typedef struct 
{
	GPIO DREQ(P1_20);
	GPIO SCI_CS(P1_22);
	GPIO SDI_CS(P1_23);
} IO_VS1053_control_S;

#endif