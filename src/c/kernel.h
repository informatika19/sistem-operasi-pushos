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
void parsePath(char *path, char *parsedPath[]);
int isPathValid(char *path, char *parentIndex, char *dirBuffer);
int getFileIdx(char *name, char parentIndex,char *dirBuffer);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
char* findFName(char *path, int *isFile);
int getMapEmptySectorCount(char *mapBuffer);
int getSectorsEmptyEntry(char *secBuffer);
char* getFileFromIdx(char idx, char *files);
void shell();
// char* tokenizeCommand(char* raw, int* commandLen);
void tokenizeCommand(char* raw, char* command, char* param);
void shell_cd(char* currentDir, char* params, char* dirBuffer);
void shell_ls(char currentDir, char* params);
void shell_cat(char* currentDir, char* params, char* dirBuffer);
void shell_ln(int currentDirIdx, char* params);
void realPath(char *absPath, char *path, char *newPath);

#endif
