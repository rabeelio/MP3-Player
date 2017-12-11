/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */

#include <stdint.h>
#include <stdio.h>
#include "io.hpp"
#include "periodic_callback.h"
#include "gpio.hpp"
#include "IO_uSD.h"
#include "IO_display.hpp"
#include "IO_VS1053.hpp"
#include "utilities.h"

#define SPEED_OBJ 15


/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_DISPATCHER_TASK_STACK_SIZE_BYTES = (512 * 3);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{

		// IO_VS1053_init();
		// IO_VS1053_setVolume();
		// IO_VS1053_setVS1053Clk();
		// LCD_init();


		// if(IO_uSD_findMP3Files() != FR_OK)
		// {
		// 	puts("ERROR: NO MP3 files found!!!");
		// }
		// else
		// {
		// 	puts("Found some MP3 files...");
		// }

		// IO_uSD_print_list();

    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}

/**
 * Below are your periodic functions.
 * The argument 'count' is the number of times each periodic task is called.
 */
void period_1Hz(uint32_t count)
{
	LE.toggle(1);
	// static uint16_t rpm_clicks = 0;
	// rpm_clicks+=1; 
	// send_data(SPEED_OBJ,0,(uint16_t)rpm_clicks);
	// if(IO_uSD_scanDirectories() == FR_OK)
	// {
	// 	puts("success");
	// }



	
}

void period_10Hz(uint32_t count)
{
    LE.toggle(2);
    	// char buffer[512] = {'0'};
    	// bool fin = IO_uSD_isFinishedReading();
    	// if(fin == false)
    	// {
	    // 	if((IO_uSD_readFile(buffer, 0) != FR_OK))
	    // 	{
	    // 		puts("error");
	    // 	}
	    // }
	    // else
	    // {
	    // 		puts("finished");
	    // }
    	// if (oneTime == false)
    	// {
    	// 	IO_uSD_readFile(buffer, 0);
    	// 	oneTime = true;
    	// }
    	// // // // 512 - 32 = 480 -> offset = 32
    	// // // // increment the pointer such that it's pointing to the element set by the offset
    	// // // // char buffer[512]; by definition, buffer is pointing to the first element in the array -> whatever buffer[0] is
    	// // // // *(buffer++) // increment pointer and then dereference that value at that location

    	// if (bufferOffset <= 511)
    	// {
    	// 	if(IO_VS1053_writeSDI(buffer, &bufferOffset))
	    // 	{
	    // 		// puts("wrote to VS1053");
	    // 		// printf("offset: %i\n", bufferOffset);
	    // 	}
	    // 	else
	    // 	{
	    // 		puts("failed to write to VS1053");
	    // 	}
    	// }
    	// else
    	// {
	    // 		IO_uSD_readFile(buffer, 0);
	    // 		oneTime = false;
	    // 		bufferOffset = 0;
    	// }
}
static uint16_t bufferIndex = 0;

void period_100Hz(uint32_t count)
{
    	// if (oneTime == false)
    	// {
    	// 	IO_uSD_readFile(buffer, 0);
    	// 	oneTime = true;
    	// }
    	// // // 512 - 32 = 480 -> offset = 32
    	// // // increment the pointer such that it's pointing to the element set by the offset
    	// // // char buffer[512]; by definition, buffer is pointing to the first element in the array -> whatever buffer[0] is
    	// // // *(buffer++) // increment pointer and then dereference that value at that location

    	// if (bufferOffset <= 511)
    	// {
    	// 	if(IO_VS1053_writeSDI(buffer, &bufferOffset))
	    // 	{
	    // 		// LE.on(3);
	    // 		// puts("wrote to VS1053");
	    // 		// printf("offset: %i\n", bufferOffset);
	    // 	}
	    // 	else
	    // 	{
	    // 		// LE.off(3);
	    // 		// puts("failed to write to VS1053");
	    // 	}
    	// }
    	// else
    	// {
	    // 		IO_uSD_readFile(buffer, 0);
	    // 		oneTime = false;
	    // 		bufferOffset = 0;
    	// }

		// if (oneTime == false)
  //   	{
  //   		IO_uSD_readFile(buffer, 0);
  //   		oneTime = true;
  //   	}



}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{

    	// // 512 - 32 = 480 -> offset = 32
    	// // increment the pointer such that it's pointing to the element set by the offset
    	// // char buffer[512]; by definition, buffer is pointing to the first element in the array -> whatever buffer[0] is
    	// // *(buffer++) // increment pointer and then dereference that value at that location

    	// if (bufferOffset <= 1023) //1023
    	// {
    	// 	if(IO_VS1053_writeSDI(buffer, &bufferOffset))
	    // 	{
	    // 		// LE.on(3);
	    // 		// puts("wrote to VS1053");
	    // 		// printf("offset: %i\n", bufferOffset);
	    // 	}
	    // 	else
	    // 	{
	    // 		// LE.off(3);
	    // 		puts("failed to write to VS1053");
	    // 	}
    	// }
    	// else
    	// {
	    // 		// IO_uSD_readFile(buffer, 0);
	    // 		oneTime = false;
	    // 		bufferOffset = 0;
    	// }
    	// delay_ms(0.5);
    // LE.toggle(4);
}
