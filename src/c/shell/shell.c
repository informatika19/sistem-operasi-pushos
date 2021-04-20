#include "shell.h"

#include "../lib/headers/boolean.h"
#include "../lib/headers/string.h"
#include "../lib/headers/fileIO.h"
#include "../lib/headers/math.h"

int main() {
  char command[10 * MAXIMUM_CMD_LEN];  // kalo pointer aja takut error
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN];

  char hist[HIST_SIZE][MAXIMUM_ARGC * MAXIMUM_CMD_LEN];

  char username[7], cwdName[FILE_NAME_LENGTH], promptHead[3], prompt[27], atSymb[2];
  char *cwdIdx = 0xFF;

  int argc, histc = 0, i, cmd, *err, k = 0;

  strncpy(username, "pushOS", 7);
  atSymb[0] = '@';
  atSymb[1] = 0;
  cwdName[0] = '/';
  cwdName[1] = 0;
  promptHead[0] = '>';
  promptHead[1] = ' ';
  promptHead[2] = 0;  // default prompt: "pushOS@/> "

  while (true) {
    // removeFile("temp", &err, 0x00);
    printNumber(k);
    k++;
    printString("\r\n");
    // set prompt
    clear(prompt, 27);
    strncat(prompt, username, strlen(username));
    strncat(prompt, atSymb, 1);
    strncat(prompt, cwdName, strlen(cwdName));
    strncat(prompt, promptHead, 2);
    interrupt(0x21, 0, prompt, 0, 0);

    interrupt(0x21, 1, command, 0, 0);
    // parse dan hasil parse
    argc = commandParser(command, argv);

    if (argc < 0) {
      printString("\r\n");
      continue;
    } else {
      interrupt(0x21, 0, "\r\n", 0, 0);
      cmd = cmdcmp(argv[0]);
      switch(cmd) {
        case 1: // cd
          if (argc != 2) {
            interrupt(0x21, 0, "Usage: cd <path/directory>\r\n", 0, 0);
          } else {
            shell_cd(&cwdIdx, argv[1], cwdName);
          }
          break;
        case 2: // ls
          if (argc == 2) {
            shell_ls(cwdIdx, argv[1]);
          } else if (argc == 1) {
            shell_ls(cwdIdx, 0);
          } else {
            interrupt(0x21, 0, "Usage: ls <path/directory> or ls", 0, 0);
          }
          break;
        case 5: // cwd
          printNumber(cwdIdx);
          printString(" - ");
          printString(cwdName);
          printString("\r\n");
          break;
        case 6: // history
          for (i = 0; i < HIST_SIZE; i++) {
            if (strlen(hist[i]) != 0) {
              printString(hist[i]);
              printString("\r\n");
            }
          }
          break;
        case 3: // cat
          setParameter(cwdIdx, argv);
          if (argc != 2) {
            interrupt(0x21, 0, "Usage: cat <path/file>\r\n", 0, 0);
          } else {
            interrupt(0x21, 0x0006, "cat", 0x3001, &err, 0);
          }
          break;
        case 4: // ln
          setParameter(cwdIdx, argv);
          if (argc != 3) {
            interrupt(0x21, 0, "Usage: ln <path/src> <path/dest>\r\n", 0, 0);
          } else {
            interrupt(0x21, 0x0006, "ln", 0x3001, &err, 0);
          }
          break;
        case 7: // cp
          setParameter(cwdIdx, argv);
          if (argc != 3) {
            interrupt(0x21, 0, "Usage: cp <path/src> <path/dest>\r\n", 0, 0);
          } else {
            interrupt(0x21, 0x0006, "cp", 0x3001, &err, 0);
          }
          break;
        case 8: // mv
          setParameter(cwdIdx, argv);
          break;
        case 9: // rm
          setParameter(cwdIdx, argv);
          break;
        case 10: // mkdir
          setParameter(cwdIdx, argv);
          break;
        default: // -1
          interrupt(0x21, 0, "Unknown command ", 0, 0);
          interrupt(0x21, 0, argv[0], 0, 0);
          interrupt(0x21, 0, "\r\n", 0, 0);
      }
      interrupt(0x21, 0, "\r", 0, 0);
    }

    // HISTORY
    histc = (histc >= HIST_SIZE) ? 0 : histc;
    for (i = 1; i < HIST_SIZE; i++) {
      strcpy(hist[i - 1], hist[i]);
    }
    strcpy(hist[HIST_SIZE - 1], command);
    histc++;
    printString("\r");
  }
}

int cmdcmp(char *argv) {
  if (strncmp("cd", argv, MAXIMUM_CMD_LEN) == 0) return 1;
  if (strncmp("ls", argv, MAXIMUM_CMD_LEN) == 0) return 2;
  if (strncmp("cat", argv, MAXIMUM_CMD_LEN) == 0) return 3;
  if (strncmp("ln", argv, MAXIMUM_CMD_LEN) == 0) return 4;
  if (strncmp("cwd", argv, MAXIMUM_CMD_LEN) == 0) return 5;
  if (strncmp("history", argv, MAXIMUM_CMD_LEN) == 0) return 6;
  if (strncmp("cp", argv, MAXIMUM_CMD_LEN) == 0) return 7;
  if (strncmp("mv", argv, MAXIMUM_CMD_LEN) == 0) return 8;
  if (strncmp("rm", argv, MAXIMUM_CMD_LEN) == 0) return 9;
  if (strncmp("mkdir", argv, MAXIMUM_CMD_LEN) == 0) return 10;
  return -1;
}

int commandParser(char *cmd, char *argument) {
  int i, j;
  bool stop = false;

  i = 0, j = 0;
  for (; *cmd == ' '; cmd++);
  stop = strcmp(cmd, "\0") == 0; // hanya enter
  while (*cmd != '\0' && !stop) {
    stop = i >= MAXIMUM_CMD_LEN;
    switch (*cmd) {
      case ' ':
        *(argument + j + i) = 0;
        j += MAXIMUM_CMD_LEN * (i != 0);
        i = 0;
        break;
      case '\\':
        cmd++;
        *(argument + j + i) = *(cmd);
        stop = cmd == 0;  // \ di akhir string
        i += 1 * !stop;
        break;
      default:
        *(argument + j + i) = *cmd;
        i++;
    }
    cmd++;
  }

  *(argument + j + i) = 0;
  return stop ? -1 : (div(j, MAXIMUM_CMD_LEN) + 1);
}

void shell_cd(char *parentIndex, char *path, char *newCwdName) {
  char dir[2 * SECTOR_SIZE];
  int tmpPI = *parentIndex, test;
  bool found = false, isDir = true;

  if (strncmp(path, ".", MAXIMUM_CMD_LEN)) {
    if (strncmp(path, "/", MAXIMUM_CMD_LEN) != 0) {
      interrupt(0x21, 0x0002, dir, ROOT_SECTOR, 0);  // readSector
      interrupt(0x21, 0x0002, dir + SECTOR_SIZE, ROOT_SECTOR+1, 0);

      test = getFileIndex(path, *parentIndex, dir);
      tmpPI = test & 0xFF;
      found = test > -1;
    } else {
      found = true;
      tmpPI = 0xFF;
    }

    if (found) {
      isDir = *(dir + (tmpPI * 0x10) + 1) == '\xFF';
      if (tmpPI == 0xFF) {  // shell_cd ke root
          *parentIndex = 0xFF;
          strncpy(newCwdName, "/", 14);
      } else if (isDir) {
          *parentIndex = tmpPI;
          strncpy(newCwdName, dir + (tmpPI * 0x10) + 2, 14);
      } else {
          interrupt(0x21, 0, path, 0, 0);
          interrupt(0x21, 0, " is not a directory.\r\n", 0, 0);
      }
    } else {
      interrupt(0x21, 0, "Directory ", 0, 0);
      interrupt(0x21, 0, path, 0, 0);
      interrupt(0x21, 0, " not found.\r\n", 0, 0);
    }
  }
  return;
}

void shell_ls(char parentIndex, char* folder) {
  int i, j;
  bool found = false;
  char dir[2 * SECTOR_SIZE];

  interrupt(0x21, 0x0002, dir, ROOT_SECTOR, 0);  // readSector
  interrupt(0x21, 0x0002, dir + SECTOR_SIZE, ROOT_SECTOR+1, 0);

  if (strncmp(&folder, "", 1) == 0) {
    i = 0;
    while (i < 2 * SECTOR_SIZE) {
      if (*(dir + i) == parentIndex && *(dir + i + 2) != 0) {
          interrupt(0x21, 0, dir + i + 2, 0, 0);
          if (*(dir + i + 1) == '\xFF') interrupt(0x21, 0, "/", 0, 0);
          interrupt(0x21, 0, "\r\n", 0, 0);
      }
      i += 16;
    }
  } else {
    for (i = 0; i < FILE_ENTRY_TOTAL; i++) {
      if (dir[FILE_ENTRY_LENGTH * i] == parentIndex
          && dir[FILE_ENTRY_LENGTH * i + 2] != 0
          && dir[FILE_ENTRY_LENGTH * i + 1] == '\xFF'
          && strncmp(dir[FILE_ENTRY_LENGTH * i + 2], *folder, FILE_NAME_LENGTH) == 0) { 
        found = true;
        j = i;
        break;
      }
    }
    if (!found) {
      interrupt(0x21, 0, "There is no folder named ", 0, 0);
      interrupt(0x21, 0, folder, 0, 0);
      interrupt(0x21, 0, " in this directory\r\n", 0, 0);
      return;
    } else {
      while (i < 2 * SECTOR_SIZE) {
      if (*(dir + i) == j && *(dir + i + 2) != 0) {
          interrupt(0x21, 0, dir + i + 2, 0, 0);
          if (*(dir + i + 1) == '\xFF') interrupt(0x21, 0, "/", 0, 0);
          interrupt(0x21, 0, "\r\n", 0, 0);
        }
      i += 16;
      }
    }
  }
}
