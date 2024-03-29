
#include "stdio.h"
#include "stdlib.h"
#include "uart2.hpp"
#include "utilities.h"
#include <string.h>
#include "IO_display.hpp"
#include "IO_VS1053.hpp"
#include "semphr.h"
#include "scheduler_task.hpp"


extern SemaphoreHandle_t testSem;
SemaphoreHandle_t testSem = 0;

extern SemaphoreHandle_t nextSong;
SemaphoreHandle_t nextSong = 0;

extern SemaphoreHandle_t previousSong;
SemaphoreHandle_t previousSong = 0;




#define LCD_UART_BAUD	(9600)
#define LCD_TX_SIZE		(1000)
#define LCD_RX_SIZE		(1000)

enum
{
	LCD_BEARING_TEXT 	= 0x0,
};

void LCD_init(void)
{
	testSem = xSemaphoreCreateBinary();
	nextSong = xSemaphoreCreateBinary();
	previousSong = xSemaphoreCreateBinary();

	Uart2::getInstance().init(LCD_UART_BAUD, LCD_TX_SIZE, LCD_RX_SIZE);
	return;
}

static void received_data(char *rxBuffer)
{
	Uart2::getInstance().getChar(rxBuffer, 100);
}

static bool if_ACK(void)
{
	char buf;
	received_data(&buf);
	if(buf == 6)
	{
		return true;
	}
	else
	{
		return false;
	}

}

static bool if_NACK(void)
{
	char buf;
	received_data(&buf);
	if(buf == 21)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void sliderISR(void)
{
	puts("fired");
	xSemaphoreGiveFromISR(testSem, NULL);
}

static int hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

void read_slider(void)
{

	// portDISABLE_INTERRUPTS();
	int checksum;
	int read_cmd = 0;
	int object_ID = 4;
	int data = 0;
	char buf;

	// Uart2::getInstance().putChar(read_cmd, 0);
	// Uart2::getInstance().putChar(object_ID, 0);
	// Uart2::getInstance().putChar(data, 0);
	// checksum = read_cmd ^ object_ID ^ 0 ^ data;
	// Uart2::getInstance().putChar(checksum, 0);

	// // if (if_ACK())
	// // {
		// taskDISABLE_INTERRUPTS();
		printf("Successfully transmitted data!\n");
		received_data(&buf);
		// printf("byte 1: %02x\n", buf);
		received_data(&buf);
		// printf("byte 2: %02x\n", buf);
		received_data(&buf);
		// printf("byte 3: %02x\n", buf);
		received_data(&buf);
		// printf("byte 4: %02x\n", buf);
		received_data(&buf);
		printf("byte 5: %i\n", (uint8_t)buf);

		IO_VS1053_setVolume((uint8_t)buf);
		received_data(&buf);
		// printf("byte 6: %02x\n", buf);
		// taskENABLE_INTERRUPTS();

	// }
	// else if (if_NACK())
	// {
	// 	printf("NACK received.\n");
	// }
	// else
	// {
	// 	printf("Invalid data sent to LCD.\n");
	// }
	// puts("fired");
		// portENABLE_INTERRUPTS();

}

void send_data(char object_ID, char object_index, int data)
{
	int checksum;
	int write_cmd = 1;
	char msb, lsb;
	lsb = data & 0xff;
	msb = (data >> 8) & 0xff;

	Uart2::getInstance().putChar(write_cmd, 0);
	Uart2::getInstance().putChar(object_ID, 0);
	Uart2::getInstance().putChar(object_index, 0);
	Uart2::getInstance().putChar(msb, 0);
	Uart2::getInstance().putChar(lsb, 0);
	checksum = write_cmd ^ object_ID ^ object_index ^ 0 ^ data;
	Uart2::getInstance().putChar(checksum, 0);

	if (if_ACK())
	{
		// printf("Successfully transmitted data!\n");
	}
	else if (if_NACK())
	{
		// printf("NACK received.\n");
	}
	else
	{
		// printf("Invalid data sent to LCD.\n");
	}

	return;
}

char return_char[5] = {0};

static char * numbers_to_ASCII(uint32_t number, int * counter)
{
	int number_1, number_2, number_3;

	memset(&return_char[0], 0x00, 5*(sizeof(char)));

	number_1 = number / 100;
	number_2 = (number - (100 * number_1)) / 10;
	number_3 = (number - (100 * number_1) - (10 * number_2)) / 1;

	return_char[(*counter)++] = number_1 + '0';
	return_char[(*counter)++] = number_2 + '0';
	return_char[(*counter)++] = number_3 + '0';

	return &return_char[0];
}

void send_bearing_to_LCD(uint32_t bearing_value, bool gps_locked)
{
	char object_index = LCD_BEARING_TEXT;
	int checksum = 0;
	int counter = 0;
	int write_cmd = 2;
	
	char *char_string = numbers_to_ASCII(bearing_value, &counter);

	if (gps_locked)
	{
		Uart2::getInstance().putChar(write_cmd, 0);
		Uart2::getInstance().putChar(object_index, 0);
		Uart2::getInstance().putChar(counter+1, 0);

		for (int i = 0; i < counter; i++)
		{
			Uart2::getInstance().putChar(char_string[i], 0);
			checksum ^= char_string[i];
		}

		Uart2::getInstance().putChar(0x0, 0);
		checksum ^= (write_cmd ^ object_index ^ (counter + 1) ^ 0x00);
		Uart2::getInstance().putChar(checksum, 0);
	}
	else
	{
		// Hardcoding the text in the LCD to "Invalid"
		Uart2::getInstance().putChar(write_cmd, 0);
		Uart2::getInstance().putChar(object_index, 0);
		Uart2::getInstance().putChar(0x09, 0);
		Uart2::getInstance().putChar(0x69, 0);
		Uart2::getInstance().putChar(0x6E, 0);
		Uart2::getInstance().putChar(0x76, 0);
		Uart2::getInstance().putChar(0x61, 0);
		Uart2::getInstance().putChar(0x6C, 0);
		Uart2::getInstance().putChar(0x69, 0);
		Uart2::getInstance().putChar(0x64, 0);
		Uart2::getInstance().putChar(0x21, 0);
		Uart2::getInstance().putChar(0x00, 0);
		Uart2::getInstance().putChar(0x5B, 0);

	}
}
static bool pratham = false; 

void playButton(void)
{
	// Our parameter was the orientation tasks' pointer, but you may want to check for NULL pointer first.
    scheduler_task *task1 = scheduler_task::getTaskPtrByName("playMP3");

    pratham = pratham ? false : true;

    // You can use FreeRTOS API or the wrapper resume() or suspend() methods
    if(!pratham)
    {
     	vTaskSuspend(task1->getTaskHandle());  // Can also use: compute->resume();
     	printf("suspending\n");
    }
    else
    {
    	vTaskResume(task1->getTaskHandle());  // Can also use: compute->resume();
    	printf("resuming\n");
    }
    
	// printf("fired\n");
}

void fctNextSong(void)
{
	// close current song
	// decrement sonsg 
	xSemaphoreGiveFromISR(nextSong, NULL);
	// puts("next");
}

void fctPreviousSong(void)
{
	// close current song
	// decrement song
	xSemaphoreGiveFromISR(previousSong, NULL); 
	// puts("previous");
}

void send_static_text(char object_index, char * string)
{
	int write_cmd = 2;
	int checksum = 0;

	Uart2::getInstance().putChar(write_cmd, 0);
	Uart2::getInstance().putChar(object_index, 0);
	Uart2::getInstance().putChar((strlen(string)+1), 0);

    for (int i = 0; i < strlen(string); i++)
    {
        Uart2::getInstance().putChar((int)string[i], 0);
        checksum ^= (int)string[i];
    }

    Uart2::getInstance().putChar(0x0, 0);
	checksum ^= (write_cmd ^ object_index ^ (strlen(string)+1) ^ 0x00);
	Uart2::getInstance().putChar(checksum, 0);
}
