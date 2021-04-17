#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "lib/headers/math.h"
#include "lib/headers/string.h"
#include "lib/headers/logo.h"
#include "lib/headers/fileIO.h"
#include "lib/headers/folderIO.h"
#include "lib/headers/boolean.h"
#include "shell/shell.h"

void handleInterrupt21(int AX, int BX, int CX, int DX);
void executeProgram(char *filename, int segment, int *success, char parentIndex);
void clear(char *buffer, int length);

#endif
