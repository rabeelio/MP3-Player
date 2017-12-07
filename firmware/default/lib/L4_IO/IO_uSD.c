#include "IO_uSD.h"
#include "IO_uSD_componentSpecific.h"

extern const IO_uSD_config_S IO_uSD_config;
IO_uSD_data_S IO_uSD_data;

void IO_uSD_readSector(void)
{

}

void IO_uSD_init(void)
{
    IO_uSD_data.path[0] = IO_uSD_config.drivePath[0]; // 0 or 1 - depending on config
    IO_uSD_data.path[1] = IO_uSD_config.drivePath[1]; // :
    IO_uSD_data.path[2] = IO_uSD_config.drivePath[2]; // /0 - NULL terminator

        char path[256];
    strcpy(path, "1:");
    scan_files(path);

}

// FRESULT IO_uSD_scanDirectories(void)
// {
//     FRESULT returnCode  = FR_INVALID_PARAMETER;
//     FRESULT res;
//     DIR dir;
//     static FILINFO fno;
//     UINT i = 0;

//     // open the directory by passing a directory object and the path to the uSD 
//     returnCode = f_opendir(&dir, IO_uSD_data.path); 

//     // check if we were able to open the directory 
//     if (returnCode == FR_OK)
//     {
//         while(1)
//         {
//             // attempt to read the directory 
//             returnCode = f_readdir(&dir, &fno);

//             // check if there was an error or if at the end of the directory 
//             if (returnCode != FR_OK || fno.fname[0] == 0)
//             {
//                 break;
//             }

//             if (fno.fattrib & AM_DIR)
//             {
//                 i = strlen(IO_uSD_data.path); // excludes NULL terminator so 2 chars
//                 sprintf(&IO_uSD_data.path[i], "/%s", fno.fname); // e.g., 1:/MP3Folder/0 -- NOTE: sprintf adds NULL terminator
//                 returnCode = IO_uSD_scanDirectories(); //scan_files(1:/MP3Folder/0);

//                 if (returnCode == FR_OK) // won't be OK once we reach end of a directory
//                 {
//                     break;
//                 }
//                 else // time to move onto the next directory if it exists
//                 {
//                     IO_uSD_data.path[i] = 0;
//                 }
//             }
//             else // found a file, not directory 
//             {
//                 printf("%s/%s\n", IO_uSD_data.path, fno.fname);
//             }
//         }
//         f_closedir(&dir);
//     }

//     return returnCode;
// }

FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
            {
                puts("end of directory");
                break;  /* Break on error or end of dir */
            } 
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                puts("calling scans");
                printf("path: %s\n", path);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                printf("VALUE OF I: %i\n", i);
                puts("path = 0");
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}


