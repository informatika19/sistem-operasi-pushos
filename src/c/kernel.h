#ifndef _KERNEL_H_
#define _KERNEL_H_

void handleInterrupt21(int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length);
void printLogoASCII();
void printLogoGrafik();
void lineLogo(char * string, int y);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
char *findFName(char *path, int *isFile);
int getMapEmptySectorCount(char *mapBuffer);
int getSectorsEmptyEntry(char *secBuffer);
int isPathValid(char *path);

#endif
