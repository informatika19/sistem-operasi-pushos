#include "shell.h"

#include "../lib/headers/boolean.h"
#include "../lib/headers/string.h"
#include "../lib/headers/fileIO.h"
#include "../lib/headers/folderIO.h"
#include "../lib/headers/math.h"

int main() {
  char command[10 * 20];
  char argv[10][20];
  char hist[HIST_SIZE][10 * 20];
  char username[7], cwdName[14], cwdIdx;
  int argc, histc = 0, i, cmd, success;

  cwdIdx = 0xFF;

  clear(argv, 200);
  clear(hist, HIST_SIZE * 200);
  clear(username, 7);
  clear(cwdName, 14);

  strncpy(username, "pushOS", 7);
  cwdName[0] = '/';
  cwdName[1] = 0;

  while (true) {
    success = 0;
    getParameter(&cwdIdx, cwdName, argv, &success);
    if (success == 1) {
      clear(argv, 10 * 20);
      printString("got params\r\n");
    }

    removeFile("temp", &success, 0x00);
    if (success == 1) printString("removed file\r\n"); //x

    printString(username);
    printString("@");
    printString(cwdName);
    printString("> ");
    
    clear(command, 200);
    clear(argv, 10 * 20);

    readString(command);
    // parse dan hasil parse
    argc = commandParser(command, argv);

    printString("\r\n");
    if (argc < 0) {
      continue;
    } else {
      cmd = cmdcmp(argv[0]);
      switch(cmd) {
        case 1: // cd
          if (argc != 2) {
            printString("Usage: cd <path/directory>\r\n");
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
            printString("Usage: ls <path/directory> or ls");
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
          setParameter(cwdIdx, cwdName, argv, &success);
          if (argc != 2 || !success) {
            printString("Usage: cat <path/file>\r\n");
          } else {
            exec("cat", 0x3001, &success, 0x00);
          }
          break;
        case 4: // ln
          setParameter(cwdIdx, cwdName, argv, &success);
          if (argc != 3 || !success) {
            printString("Usage: ln <path/src> <path/dest>\r\n");
          } else {
            exec("ln", 0x3001, &success, 0x00);
          }
          break;
        case 7: // cp
          setParameter(cwdIdx, cwdName, argv, &success);
          if (argc != 3 || !success) {
            printString("Usage: cp <path/src> <path/dest>\r\n");
          } else {
            exec("cp", 0x3001, &success, 0x00);
          }
          break;
        case 8: // mv
          setParameter(cwdIdx, cwdName, argv, &success);
          break;
        case 9: // rm
          setParameter(cwdIdx, cwdName, argv, &success);
          if (argc != 2 || !success) {
            printString("Usage: rm <path/src> <path/dest>\r\n");
          } else {
            exec("rm", 0x3001, &success, 0x00);
          }
          break;
        case 10: // mkdir
          setParameter(cwdIdx, cwdName, argv, &success);
          if (argc != 2 || !success) {
            printString("Usage: mkdir <foldername>\r\n");
          } else {
            exec("mkdir", 0x3001, &success, 0x00);
          }
          break;
        default: // -1
          printString("Unknown command ");
          printString(argv[0]);
          printString("\r\n");
      }
      printString("\r\n");
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
  if (strncmp("cd", argv, 20) == 0) return 1;
  if (strncmp("ls", argv, 20) == 0) return 2;
  if (strncmp("cat", argv, 20) == 0) return 3;
  if (strncmp("ln", argv, 20) == 0) return 4;
  if (strncmp("cwd", argv, 20) == 0) return 5;
  if (strncmp("history", argv, 20) == 0) return 6;
  if (strncmp("cp", argv, 20) == 0) return 7;
  if (strncmp("mv", argv, 20) == 0) return 8;
  if (strncmp("rm", argv, 20) == 0) return 9;
  if (strncmp("mkdir", argv, 20) == 0) return 10;
  return -1;
}

int commandParser(char *cmd, char *argument) {
  int i, j;
  bool stop = false;

  i = 0, j = 0;
  for (; *cmd == ' '; cmd++);
  stop = strcmp(cmd, "\0") == 0; // hanya enter
  while (*cmd != '\0' && !stop) {
    stop = i >= 20;
    switch (*cmd) {
      case ' ':
        *(argument + j + i) = 0;
        j += 20 * (i != 0);
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
  return stop ? -1 : (div(j, 20) + 1);
}

void shell_cd(char *parentIndex, char *path, char *newCwdName) {
  char dir[2 * 512];
  int tmpPI = *parentIndex, test;
  bool found = false, isDir = true;

  if (strncmp(path, ".", 20)) {
    if (strncmp(path, "/", 20) != 0) {
      interrupt(0x21, 0x0002, dir, 0x101, 0);  // readSector
      interrupt(0x21, 0x0002, dir + 512, 0x102, 0);

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
          printString(path);
          printString(" is not a directory.");
      }
    } else {
      printString("Directory ");
      printString(path);
      printString(" not found.");
    }
  }
  return;
}

void shell_ls(char parentIndex, char* folder) {
  int i, j;
  bool found = false;
  char dir[2 * 512];

  interrupt(0x21, 0x0002, dir, 0x101, 0);  // readSector
  interrupt(0x21, 0x0002, dir + 512, 0x102, 0);

  if (folder == 0) {
    i = 0;
    while (i < 2 * 512) {
      if (*(dir + i) == parentIndex && *(dir + i + 2) != 0) {
        printString(dir + i + 2);
        if (*(dir + i + 1) == '\xFF') printString("/");
        printString("\r\n");
      }
      i += 16;
    }
  } else {
    for (i = 0; i < 64; i++) {
      if (*(dir + 16 * i) == parentIndex
          && *(dir + 16 * i + 2) != 0
          && *(dir + 16 * i + 1) == '\xFF'
          && strncmp((dir + 16 * i + 2), folder, 14) == 0) { 
        found = true;
        j = i;
        break;
      }
    }
    if (!found) {
      printString("There is no folder named ");
      printString(folder);
      printString(" in this directory");
      return;
    } else {
      while (i < 2 * 512) {
      if (*(dir + i) == j && *(dir + i + 2) != 0) {
          printString(dir + i + 2);
          if (*(dir + i + 1) == '\xFF') printString("/");
          printString("\r\n");
        }
      i += 16;
      }
    }
  }
}
