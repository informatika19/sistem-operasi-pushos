#ifndef _KERNEL_H_
#define _KERNEL_H_

void handleInterrupt21(int AX, int BX, int CX, int DX);
void executeProgram(char *filename, int segment, int *success, char parentIndex);
void clear(char *buffer, int length);

#endif
