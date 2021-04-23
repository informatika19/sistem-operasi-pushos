#ifndef _FOLDER_IO_H_
#define _FOLDER_IO_H_

#define FILE_NAME_LENGTH    14
#define FILE_ENTRY_LENGTH   16
#define FILE_ENTRY_TOTAL    64

#define SECTOR_SIZE         512
#define SECTOR_ENTRY_LENGTH 16
#define SECTOR_FILE_TOTAL   32
#define SECTOR_TOTAL        2880

#define MAP_SECTOR          0x100
#define ROOT_SECTOR         0x101
#define SECTORS_SECTOR      0x103

#define MAXIMUM_CMD_LEN     20
#define MAXIMUM_ARGC        10

// need to protect bin
void createFolder();
void deleteFolder();

#endif
