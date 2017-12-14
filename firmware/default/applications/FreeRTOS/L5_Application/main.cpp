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
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "periodic_scheduler/periodic_callback.h"
#include "uart2.hpp"
#include "uart3.hpp"
#include "utilities.h"
#include "stdio.h"
#include "stdlib.h"
#include "IO_VS1053.hpp"
#include "IO_uSD.h"
#include "eint.h"
#include "IO_display.hpp"
#define SPEED_OBJ 15

static char buffer[512];
static uint16_t bufferOffset = 0;
static bool oneTime = false;
static uint8_t num = 0;
static char screenText[] = "sunny leone";
extern SemaphoreHandle_t testSem;
extern SemaphoreHandle_t nextSong;
extern SemaphoreHandle_t previousSong;

// static

// display task read/write -> sends queue data to playMP3 task 

class prevSongTask : public scheduler_task{
public: 
    prevSongTask(uint8_t priority) : scheduler_task("prevSongTask", 1024, priority, NULL)
    {

    }
    bool run(void *p)
    {
        if(xSemaphoreTake(previousSong, portMAX_DELAY))
        {
            if (num == 0)
            {

            }
            else
            {
                num--;
            }


            if(num >= 0)
            {
                send_static_text(0, getSongName(num));
                IO_uSD_closeFile();
                oneTime = false;
                bufferOffset = 0;
            }
            else
            {
                num = 0;
            }
            printf("previous: %i\n", num);
        }

        return true;
    }
};


class nextSongTask : public scheduler_task{
public: 
    nextSongTask(uint8_t priority) : scheduler_task("nextSongTask", 1024, priority, NULL)
    {

    }
    bool run(void *p)
    {
        if(xSemaphoreTake(nextSong, portMAX_DELAY))
        {
            num++;
            if(num <= 9)
            {
                send_static_text(0, getSongName(num));
                IO_uSD_closeFile();
                oneTime = false;
                bufferOffset = 0;
            }
            else 
            {
                num = 9;
            }
            printf("next: %i\n", num);
        }
        return true;
    }
};


class volume: public scheduler_task{
public: 
    volume(uint8_t priority) : scheduler_task("volume", 1024, priority, NULL)
    {

    }
    bool run(void *p)
    {
        if(xSemaphoreTake(testSem, portMAX_DELAY))
        {
            
            read_slider();
            
            puts("got it!");
        }
        return true;
    }
};

class display: public scheduler_task{
public:
    display(uint8_t priority) : scheduler_task("display",2048,priority,NULL)
    {
        LCD_init();
        send_static_text(0, getSongName(num));

    }

    bool run(void *p)
    {
        
        // read_slider();
        // send_static_text(1, screenText);
        // send_static_text(2, screenText);
        // static uint16_t rpm_clicks = 0;
        // rpm_clicks+=1; 
        // send_data(SPEED_OBJ,0,(uint16_t)rpm_clicks);
        vTaskSuspend(NULL);
        return true;
    }
};

class playMP3: public scheduler_task{
public:
    playMP3(uint8_t priority) : scheduler_task("playMP3",2048,priority,NULL)
    {
        IO_VS1053_init();
        IO_VS1053_setVolume(100);
        IO_VS1053_setVS1053Clk();
        

        if(IO_uSD_findMP3Files() != FR_OK)
        {
            puts("ERROR: NO MP3 files found!!!");
        }
        else
        {
            puts("Found some MP3 files...");
        }

        IO_uSD_print_list();
    }

    bool run(void *p){

        if (IO_uSD_isFinishedReading())
        {
            puts("finished");
            num++;
            taskENTER_CRITICAL();
            send_static_text(0, getSongName(num));
            taskEXIT_CRITICAL();

            if (num >= 9)
            {
                num = 0;
            }
            IO_uSD_clearSongFin();
        }

        if (oneTime == false)
        {
             IO_uSD_readMP3(buffer, num);
             oneTime = true;

        }

        if (bufferOffset <= 511) //
        {
             if(IO_VS1053_writeSDI(buffer, &bufferOffset))
             {
                 // puts("wrote to VS1053");
                 // printf("offset: %i\n", bufferOffset);
             }
             else
             {
                 // puts("failed to write to VS1053");
             }
        }
        else
        {
             // IO_uSD_readFile(buffer, 0);
             oneTime = false;
             bufferOffset = 0;
        }
        vTaskDelay(1);
        return true;
    }

};




/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at scheduler_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */
int main(void)
{

    eint3_enable_port0(29, eint_rising_edge, sliderISR);
    eint3_enable_port0(30, eint_rising_edge, playButton);
    eint3_enable_port2(0, eint_rising_edge, fctPreviousSong);
    eint3_enable_port2(1, eint_rising_edge, fctNextSong);

    /**
     * A few basic tasks for this bare-bone system :
     *      1.  Terminal task provides gateway to interact with the board through UART terminal.
     *      2.  Remote task allows you to use remote control to interact with the board.
     *      3.  Wireless task responsible to receive, retry, and handle mesh network.
     *
     * Disable remote task if you are not using it.  Also, it needs SYS_CFG_ENABLE_TLM
     * such that it can save remote control codes to non-volatile memory.  IR remote
     * control codes can be learned by typing the "learn" terminal command.
     */
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
    scheduler_add_task(new playMP3(PRIORITY_HIGH));
    scheduler_add_task(new display(PRIORITY_HIGH));
    scheduler_add_task(new volume(PRIORITY_HIGH));
    scheduler_add_task(new nextSongTask(PRIORITY_HIGH));
    scheduler_add_task(new prevSongTask(PRIORITY_HIGH));


    /* Consumes very little CPU, but need highest priority to handle mesh network ACKs */
    // scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));

    /* Change "#if 0" to "#if 1" to run period tasks; @see period_callbacks.cpp */
    #if 0
    const bool run_1Khz = false;
    scheduler_add_task(new periodicSchedulerTask(run_1Khz));
    #endif

    /* The task for the IR receiver to "learn" IR codes */
    // scheduler_add_task(new remoteTask  (PRIORITY_LOW));

    /* Your tasks should probably used PRIORITY_MEDIUM or PRIORITY_LOW because you want the terminal
     * task to always be responsive so you can poke around in case something goes wrong.
     */

    /**
     * This is a the board demonstration task that can be used to test the board.
     * This also shows you how to send a wireless packets to other boards.
     */
    #if 0
        scheduler_add_task(new example_io_demo());
    #endif

    /**
     * Change "#if 0" to "#if 1" to enable examples.
     * Try these examples one at a time.
     */
    #if 0
        scheduler_add_task(new example_task());
        scheduler_add_task(new example_alarm());
        scheduler_add_task(new example_logger_qset());
        scheduler_add_task(new example_nv_vars());
    #endif

    /**
	 * Try the rx / tx tasks together to see how they queue data to each other.
	 */
    #if 0
        scheduler_add_task(new queue_tx());
        scheduler_add_task(new queue_rx());
    #endif

    /**
     * Another example of shared handles and producer/consumer using a queue.
     * In this example, producer will produce as fast as the consumer can consume.
     */
    #if 0
        scheduler_add_task(new producer());
        scheduler_add_task(new consumer());
    #endif

    /**
     * If you have RN-XV on your board, you can connect to Wifi using this task.
     * This does two things for us:
     *   1.  The task allows us to perform HTTP web requests (@see wifiTask)
     *   2.  Terminal task can accept commands from TCP/IP through Wifly module.
     *
     * To add terminal command channel, add this at terminal.cpp :: taskEntry() function:
     * @code
     *     // Assuming Wifly is on Uart3
     *     addCommandChannel(Uart3::getInstance(), false);
     * @endcode
     */
    #if 0
        Uart3 &u3 = Uart3::getInstance();
        u3.init(WIFI_BAUD_RATE, WIFI_RXQ_SIZE, WIFI_TXQ_SIZE);
        scheduler_add_task(new wifiTask(Uart3::getInstance(), PRIORITY_LOW));
    #endif

    scheduler_start(); ///< This shouldn't return
    return -1;
}
