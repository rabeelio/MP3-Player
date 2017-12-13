#ifndef IO_VS1053_H
#define IO_VS1053_H

#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "gpio.hpp"
#include "io.hpp"
#include "ssp0.h"

void IO_VS1053_init(void);
bool IO_VS1053_writeSDI(char *buffer, uint16_t *offset);
void IO_VS1053_setVolume(uint16_t percentage);
void IO_VS1053_setVS1053Clk(void);

#endif