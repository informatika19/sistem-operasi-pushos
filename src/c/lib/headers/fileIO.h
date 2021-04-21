#ifndef _FILE_IO_H_
#define _FILE_IO_H_

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

void clearSector(int sector);
void clear(char *buffer, int length);

void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sections, char parentIndex);
void removeFile(char *path, int *result, char parentIndex);

int getFileIndex(char *path, char parentIndex, char *dir);
int parsePath(char *path, char *parents, char *fname);

void setParameter(int parentIndex, char *cwdName, char *argv, int *success);
void getParameter(int *parentIndex, char *cwdName, char *argv, int *success);

int getSectorsNeeded(char *argv);

void exec(char *path, int segment, int *success, int parentIndex);

#endif
