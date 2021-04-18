#ifndef _SHELL_H_
#define _SHELL_H_

#define MAXIMUM_CMD_LEN 20
#define HIST_SIZE 5

int shell();
int commandParser(char *cmd, char *argument);
void shell_cd(char *parentIndex, char *path, char *newCwdName);
void shell_ls(char parentIndex);

#endif
