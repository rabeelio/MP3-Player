#ifndef L5_APPLICATION_SDCARD_H_
#define L5_APPLICATION_SDCARD_H_

#include "string.h"
#include "stdio.h"
#include "ff.h"
#include "string.h"

#define THREE_BYTES 3
#define PATH_MAX_LENGTH 256
#define BUFFER_SIZE 	512 
#define MOUNT_NOW 		1

typedef struct 
{
	//FF parameters
	FRESULT returnCode;
	DIR directoryObject;
	FIL fileInfo;

	FIL dest;

	// file parameters
	char currentSong[32];
	DWORD bytesLeftToRead;
	DWORD remainderInBytes;
	DWORD offsetInBytes;
	bool isFileOpen;
	bool finishedReadingFile;
} IO_uSD_data_S;

typedef struct node 
{
	// node data
	char songName[32];
	char songPath[64];

	// pointer to next node
	struct node *next;
} IO_uSD_node_S;

typedef struct 
{
	IO_uSD_node_S *headPtr;
} IO_uSD_LL_data_S;

typedef struct 
{
	const char drivePath[THREE_BYTES]; // "1:\0"

} IO_uSD_config_S;

#ifdef __cplusplus
extern "C" {
#endif
FRESULT IO_uSD_scanDirectoriesForMP3s(char* path); 
bool IO_uSD_init(void);
bool IO_uSD_findMP3Files(void);
void IO_uSD_print_list(void);
FRESULT IO_uSD_readMP3(char *buffer, uint8_t songNum);
bool IO_uSD_isFinishedReading(void);
FRESULT IO_uSD_closeFile(void);
char* IO_uSD_getCurrentSongsName(void);
#ifdef __cplusplus
}
#endif

#endif /* L5_APPLICATION_SDCARD_H_ */