#ifndef _SHELL_H_
#define _SHELL_H_

#define MAXIMUM_CMD_LEN 20
#define HIST_SIZE 5

#include "../lib/headers/fileIO.h"
#include "../lib/headers/boolean.h"
#include "../lib/headers/string.h"
#include "utilities/headers/cat.h"
#include "utilities/headers/cp.h"
#include "utilities/headers/ln.h"
#include "utilities/headers/mkdir.h"
#include "utilities/headers/mv.h"
#include "utilities/headers/rm.h"

int shell();
int commandParser(char *cmd, char *argument);
void shell_cd(char *parentIndex, char *path, char *newCwdName);
void shell_ls(char parentIndex);

#endif
