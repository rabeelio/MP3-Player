#include "IO_uSD.h"
#include "IO_uSD_componentSpecific.h"
#include "stdlib.h"

extern const IO_uSD_config_S IO_uSD_config;
IO_uSD_LL_data_S IO_uSD_LL_data = { .headPtr = NULL };
IO_uSD_data_S IO_uSD_data = 
{
    .currentSong = "NULL",
    .bytesLeftToRead = 0,
    .remainderInBytes = 0,
    .offsetInBytes = 0,
    .isFileOpen = false,
    .finishedReadingFile = false,
};

static bool IO_uSD_checkIfMP3(char *string, uint8_t size);
static void IO_uSD_addNode(char *songName, char *songPath);
static char* getSongPath(uint8_t songNum);
static char* getSongName(uint8_t songNum);
static FRESULT IO_uSD_closeFile(void);

bool IO_uSD_init(void)
{
    // TODO: need to figure out how to mount properly; letting Preet mount for now
    // FATFS filesystemObject;
    // return f_mount(&filesystemObject, IO_uSD_config.drivePath, MOUNT_NOW);
}


bool IO_uSD_findMP3Files(void)
{
    FRESULT success = FR_INVALID_PARAMETER;

    char path[256];
    path[0] = IO_uSD_config.drivePath[0]; // 1
    path[1] = IO_uSD_config.drivePath[1]; // :
    path[2] = IO_uSD_config.drivePath[2]; // /0 - NULL terminator

    success = IO_uSD_scanDirectoriesForMP3s(path);

    return success;
}

FRESULT IO_uSD_scanDirectoriesForMP3s(char* path)
{
    FRESULT returnCode;
    DIR directoryObject;
    static FILINFO fileInfo;
    UINT stringIndex;

    // open the directory by passing a directory object and the path to the uSD 
    returnCode = f_opendir(&directoryObject, path); 

    // check if we were able to open the directory 
    if (returnCode == FR_OK)
    {
        while(1)
        {
            // attempt to read the directory 
            returnCode = f_readdir(&directoryObject, &fileInfo);

            // check if there was an error or if at the end of the directory 
            if (returnCode != FR_OK || fileInfo.fname[0] == 0)
            {
                break;
            }

            if (fileInfo.fattrib & AM_DIR)
            {
                stringIndex = strlen(path); // excludes NULL terminator so 2 chars
                sprintf(&path[stringIndex], "/%s", fileInfo.fname); // e.g., 1:/MP3Folder/0 -- NOTE: sprintf adds NULL terminator
                returnCode = IO_uSD_scanDirectoriesForMP3s(path); //scan_files(1:/MP3Folder/0);

                if (returnCode != FR_OK) // won't be OK once we reach end of a directory
                {
                    break;
                }
                else // time to move onto the next directory if it exists
                {
                    path[stringIndex] = 0;
                }
            }
            else // found a file, not directory 
            {
                uint8_t size = strlen(fileInfo.fname);
                bool isMP3 = IO_uSD_checkIfMP3(fileInfo.fname, size);
                if (isMP3)
                {
                    char filePath[64];
                    sprintf(filePath, "%s%s", path, fileInfo.fname);
                    IO_uSD_addNode(fileInfo.fname, filePath);
                }
            }
        }
        f_closedir(&directoryObject);
    }

    return returnCode;
}

static bool IO_uSD_checkIfMP3(char *string, uint8_t size)
{
    bool success = false;

    if(string[size-1] == '3' &&
       string[size-2] == 'p' &&
       string[size-3] == 'm')
    {
        success = true;
    }

    return success;

}

static IO_uSD_node_S* IO_uSD_initLL(void)
{
    // create a head ptr
    IO_uSD_node_S *head = NULL;

    // allocate heap memory for the head pointer
    head = malloc(sizeof(IO_uSD_node_S));

    // set head's data to NULL since head's only purpose is to serve as a starting point for the list
    strcpy(head->songName, "NULL");
    strcpy(head->songPath, "NULL");

    // set head's next node pointer to NULL
    head->next = NULL;

    // return head
    return head;
}

static void IO_uSD_addNode(char *songName, char *songPath)
{
    //does the list exist?
    if (IO_uSD_LL_data.headPtr == NULL)
    {
        // create the head of the LL
        IO_uSD_LL_data.headPtr = IO_uSD_initLL();
    }

    // create a node pointer and set it equal to the head pointer
    IO_uSD_node_S *currentNode = IO_uSD_LL_data.headPtr;

    // traverse the list to find the last node which is where we will add the next node
    while(currentNode->next != NULL)
    {
        currentNode = currentNode->next;
    }

    // store the address of the next node in the current node that is currently at the end of the list
    currentNode->next = malloc(sizeof(IO_uSD_node_S));
 
    // copy the relevant data into the new node whose block of memory was just allocated on the heap above
    strcpy(currentNode->next->songName, songName);
    strcpy(currentNode->next->songPath, songPath);

    // make sure the new node's next node pointer is pointing to NULL
    currentNode->next->next = NULL;
}

static void IO_uSD_initializeReadParameters(uint8_t songNum)
{
    strcpy(IO_uSD_data.currentSong, getSongName(songNum));
    IO_uSD_data.bytesLeftToRead = IO_uSD_data.fileInfo.fsize;
    IO_uSD_data.remainderInBytes = IO_uSD_data.fileInfo.fsize % BUFFER_SIZE;
}

static void IO_uSD_clearReadParameters(void)
{
    memset(&IO_uSD_data, 0, sizeof(IO_uSD_data));
}

FRESULT IO_uSD_openFile(uint8_t songNum)
{
    FRESULT returnCode;
    char destPath[] = "1:mp3copy.txt";
    const char *filePath = getSongPath(songNum);
    returnCode = f_open(&IO_uSD_data.fileInfo, filePath, FA_READ);
    // returnCode = f_open(&IO_uSD_data.dest, destPath, FA_WRITE);
    IO_uSD_initializeReadParameters(songNum);
    IO_uSD_data.isFileOpen = true;
    return returnCode;
}


    // 5, 066, 571 
    // buffer size = 512
    // 5,066,571 / 512 | remaineder will always be less than the buffer 
    // Qoutient = 9895 | Remainder = 331
    //readFIle() -> 5066571 - 512 = 5066059 | 1
    //readFile() -> 5066571 - 512 = 5065547 | 2
    // . . .
    // last chunk - 512 = 331 

    // if ()
bool IO_uSD_isFinishedReading(void)
{
    return IO_uSD_data.finishedReadingFile;
}


FRESULT IO_uSD_readFile(char *buffer, uint8_t songNum)
{
    FRESULT returnCode;
    UINT bytesRead, bytesWritten;

    if (IO_uSD_data.isFileOpen == false)
    {
        // printf("here\n");
        returnCode = IO_uSD_openFile(songNum);
        if (returnCode != FR_OK)
        {
            // puts("failed to open file.");
        }
    }

    // printf("remainder: %i\n", IO_uSD_data.remainderInBytes);
    // printf("LTR: %i\n", IO_uSD_data.bytesLeftToRead);
    // printf("offset: %i\n", IO_uSD_data.offsetInBytes);
    if (IO_uSD_data.bytesLeftToRead == IO_uSD_data.remainderInBytes)
    {
        // puts("test");
        returnCode = f_read(&IO_uSD_data.fileInfo, buffer, IO_uSD_data.remainderInBytes, &bytesRead);
        // returnCode = f_write(&IO_uSD_data.dest, buffer, bytesRead, &bytesWritten);

        if (returnCode != FR_OK)
        {
            // puts("failed to either read or write remainder");
        }
        IO_uSD_data.finishedReadingFile = true;
        IO_uSD_closeFile();
    }
    else
    {
        returnCode = f_lseek(&IO_uSD_data.fileInfo, IO_uSD_data.offsetInBytes);
        // printf("sizeofbuff: %i\n", sizeof(BUFFER_SIZE));
        returnCode = f_read(&IO_uSD_data.fileInfo, buffer, BUFFER_SIZE, &bytesRead);
        // printf("bytesread: %i\n", bytesRead);
        // returnCode = f_write(&IO_uSD_data.dest, buffer, bytesRead, &bytesWritten);
        IO_uSD_data.bytesLeftToRead -= BUFFER_SIZE;
        IO_uSD_data.offsetInBytes = IO_uSD_data.fileInfo.fsize - IO_uSD_data.bytesLeftToRead;
        // printf("buffer %s\n", buffer);
    }
    return returnCode;
}

static FRESULT IO_uSD_closeFile(void)
{
    FRESULT returnCode;
    returnCode = f_close(&IO_uSD_data.fileInfo);
    // returnCode = f_close(&IO_uSD_data.dest);
    IO_uSD_clearReadParameters();

    return returnCode;
}

// FRESULT IO_uSD_readFile(char *buffer, uint8_t songNum)
// {
//     FRESULT returnCode;

//     if (IO_uSD_data.isFileOpen == false)
//     {
//         returnCode = f_open(&IO_uSD_data.fileInfo, filePath, FA_READ);
//         IO_uSD_data.isFileOpen = true;
//         IO_uSD_initializeReadParameters(songNum);
//     }

//     // are we at EOF
//     if (f_eof(&IO_uSD_data.fileInfo) == false)
//     {
//         // read 512 bytes
//         returnCode = f_read(&IO_uSD_data.fileInfo, buffer, sizeof(buffer), &bytesRead);
//     }
//     else
//     {
//         f_close(&IO_uSD_data.fileInfo);
//         IO_uSD_clearReadParameters();
//     }
    




    // 5, 066, 571 
    // buffer size = 512
    // 5,066,571 / 512 | remaineder will always be less than the buffer 
    // Qoutient = 9895 | Remainder = 331
    //readFIle() -> 5066571 - 512 = 5066059 | 1
    //readFile() -> 5066571 - 512 = 5065547 | 2
    // . . .
    // last chunk - 512 = 331 

    // if ()



    // FRESULT returnCode;
    // FIL dest;
    // char buffer[1024];
    // UINT bytesRead, bytesWritten;
    // char destPath[] = "1:mp3copy.txt";
    // const char *filePath = getSongPath(0); // songs are 0 indexed
    // printf("file path: %s\n", filePath);
    // returnCode = f_open(&IO_uSD_data.fileInfo, filePath, FA_READ);
    // returnCode = f_open(&dest, destPath, FA_WRITE);
    // returnCode = f_read(&IO_uSD_data.fileInfo, buffer, sizeof(buffer), &bytesRead);
    // returnCode = f_write(&dest, buffer, bytesRead, &bytesWritten);
    // printf("bytes read: %i\n", bytesRead);
    // printf("bytes written: %i\n", bytesWritten);

    // f_close(&IO_uSD_data.fileInfo);
    // f_close(&dest);

//     return returnCode;
// }

static char* getSongPath(uint8_t songNum)
{
    IO_uSD_node_S *currentNode = IO_uSD_LL_data.headPtr;

    //skip head node
    currentNode = currentNode->next;

    uint8_t count = 0;
    while (currentNode != NULL)
    {
        if(count == songNum)
        {
            break;
        }
        else
        {   count++;
            currentNode = currentNode->next;
        }
    }
    printf("%s\n", currentNode->songPath);
    return currentNode->songPath;
}

static char* getSongName(uint8_t songNum)
{
    IO_uSD_node_S *currentNode = IO_uSD_LL_data.headPtr;

    //skip head node
    currentNode = currentNode->next;

    uint8_t count = 0;
    while (currentNode != NULL)
    {
        if(count == songNum)
        {
            break;
        }
        else
        {   count++;
            currentNode = currentNode->next;
        }
    }
    printf("%s\n", currentNode->songName);
    return currentNode->songName;
}

void IO_uSD_print_list(void)
{
    puts("printing node");
    IO_uSD_node_S *currentNode = IO_uSD_LL_data.headPtr;

    while(currentNode != NULL)
    {
        printf("song name: %s\n", currentNode->songName);
        printf("song path: %s\n", currentNode->songPath);
        currentNode = currentNode->next;
    }
}


