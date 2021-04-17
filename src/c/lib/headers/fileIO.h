#ifndef _FILE_IO_H_
#define _FILE_IO_H_

#define FILE_NAME_LENGTH    14
#define FILE_ENTRY_LENGTH   16
#define FILE_ENTRY_TOTAL    64

#define KERNEL_SIZE         16

#define SECTOR_SIZE         512
#define SECTOR_ENTRY_LENGTH 16
#define SECTOR_FILE_TOTAL   32
#define SECTOR_TOTAL        2880

#define MAP_SECTOR          0x100
#define ROOT_SECTOR         0x101
#define SECTORS_SECTOR      0x103

#define MAX_CD              32

#include "string.h"

void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);

void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);

void parsePath(char *path, char *parsedPath[]);
int isPathValid(char *path, char *parentIndex, char *dirBuffer);
int getFileIdx(char *name, char parentIndex,char *dirBuffer);
void findFName(char *path, int *isFile, char *fName);
int getMapEmptySectorCount(char *mapBuffer);
int getSectorsEmptyEntry(char *secBuffer);
char* getFileFromIdx(char idx, char *files);

int getFileIndex(char *path, char parentIndex, char *dir);
int parsePath1(char *path, char *parents, char *fname);

#endif
