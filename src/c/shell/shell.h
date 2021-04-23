#ifndef _SHELL_H_
#define _SHELL_H_

#define HIST_SIZE           5

int cmdcmp(char *argv);
int commandParser(char *cmd, char *argument);
void shell_cd(char *parentIndex, char *path, char *newCwdName);
void shell_ls(char parentIndex);
void shell_mkdir(char parentIndex, char *folderName);

#endif
