#ifndef L5_APPLICATION_SDCARD_H_
#define L5_APPLICATION_SDCARD_H_

#include "string.h"
#include "stdio.h"
#include "ff.h"
#include "string.h"

#define THREE_BYTES 3
#define PATH_MAX_LENGTH 256
#define SECTOR_SIZE 	512 

typedef struct 
{
	FRESULT returnCode;
	DIR directoryObject;
	FILINFO fileInfo; 

	char buffer[SECTOR_SIZE];
	char path[PATH_MAX_LENGTH];

} IO_uSD_data_S;

typedef struct 
{
	const char drivePath[THREE_BYTES]; // "1:\0"

} IO_uSD_config_S;

#ifdef __cplusplus
extern "C" {
#endif
void IO_uSD_readSector(void);
void IO_uSD_openFile(void);
FRESULT IO_uSD_scanDirectories(void); 
void IO_uSD_init(void);
FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
);
#ifdef __cplusplus
}
#endif

#endif /* L5_APPLICATION_SDCARD_H_ */