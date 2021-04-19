#include "shell.h"

#include "../lib/lib.h"

int main() {
  char command[10 * MAXIMUM_CMD_LEN];  // kalo pointer aja takut error
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN];

  char hist[HIST_SIZE][10 * 20];

  char username[11], cwdName[14], promptHead[3], prompt[27], atSymb[2];
  char *cwdIdx = 0xFF;

  int argc, histc = 0, i, cmd, *err;

  strncpy(username, "pushOS", 6);
  atSymb[0] = '@';
  atSymb[1] = 0;
  cwdName[0] = '/';
  cwdName[1] = 0;
  promptHead[0] = '>';
  promptHead[1] = ' ';
  promptHead[2] = 0;  // default prompt: "pushOS@/> "


  while (true) {
    getParameter(&cwdIdx, argv);
    if (strncpy(&argv[1], 0xFF, 1) == 0) clear(argv[1], 1);

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
      // printString("gamasuk euy\r\n")
      printString("\r");
      continue;
    } else {
      setParameter(cwdIdx, argv);
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
          shell_ls(cwdIdx);
          break;
        case 3: // cat
          if (argc != 2) {
            interrupt(0x21, 0, "Usage: cat <path/file>\r\n", 0, 0);
          } else {
            interrupt(0x21, 0x0006, "cat", 0x3000, &err, 0);
          }
          break;
        case 4: // ln
          if (argc != 3) {
            interrupt(0x21, 0, "Usage: ln <path/src> <path/dest>\r\n", 0, 0);
          } else {
            interrupt(0x21, 0x0006, "ln", 0x3000, &err, 0);
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
        case 7: // cp
          if (argc != 3) {
            interrupt(0x21, 0, "Usage: cp <path/src> <path/dest>\r\n", 0, 0);
          } else {
            interrupt(0x21, 0x0006, "cp", 0x3000, &err, 0);
          }
          break;
        case 8: // mv
          break;
        case 9: // rm
          break;
        case 10: // mkdir
          break;
        default: // -1
          interrupt(0x21, 0, "Unknown command ", 0, 0);
          interrupt(0x21, 0, argv[0], 0, 0);
          interrupt(0x21, 0, "\r\n", 0, 0);
      }
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

int cmdcmp(char *cmd) {
  if (strncmp("cd", *cmd, MAXIMUM_CMD_LEN) == 0) return 1;
  if (strncmp("ls", *cmd, MAXIMUM_CMD_LEN) == 0) return 2;
  if (strncmp("cat", *cmd, MAXIMUM_CMD_LEN) == 0) return 3;
  if (strncmp("ln", *cmd, MAXIMUM_CMD_LEN) == 0) return 4;
  if (strncmp("cwd", *cmd, MAXIMUM_CMD_LEN) == 0) return 5;
  if (strncmp("history", *cmd, MAXIMUM_CMD_LEN) == 0) return 6;
  if (strncmp("cp", *cmd, MAXIMUM_CMD_LEN) == 0) return 7;
  if (strncmp("mv", *cmd, MAXIMUM_CMD_LEN) == 0) return 8;
  if (strncmp("rm", *cmd, MAXIMUM_CMD_LEN) == 0) return 9;
  if (strncmp("mkdir", *cmd, MAXIMUM_CMD_LEN) == 0) return 10;
  return -1;
}

int commandParser(char *cmd, char *argument) {
  int i, j;
  bool stop = false;

  i = 0, j = 0;
  for (; *cmd == ' '; cmd++);
  stop = cmd == '\n';
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
          interrupt(0x21, 0, " bukan direktori.\r\n", 0, 0);
      }
    } else {
      interrupt(0x21, 0, "Direktori ", 0, 0);
      interrupt(0x21, 0, path, 0, 0);
      interrupt(0x21, 0, " tidak ditemukan.\r\n", 0, 0);
    }
  }
  return;
}

// TODO: ls ke directory lain
void shell_ls(char parentIndex) {
  int i = 0;
  char dir[2 * SECTOR_SIZE];

  interrupt(0x21, 0x0002, dir, ROOT_SECTOR, 0);  // readSector
  interrupt(0x21, 0x0002, dir + SECTOR_SIZE, ROOT_SECTOR+1, 0);

  while (i < 2 * SECTOR_SIZE) {
    if (*(dir + i) == parentIndex && *(dir + i + 2) != 0) {
        interrupt(0x21, 0, dir + i + 2, 0, 0);
        if (*(dir + i + 1) == '\xFF') interrupt(0x21, 0, "/", 0, 0);
        interrupt(0x21, 0, "\r\n", 0, 0);
    }
    i += 16;
  }
}
