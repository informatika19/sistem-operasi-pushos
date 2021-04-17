#include "shell.h"

int shell() {
  char command[10 * MAXIMUM_CMD_LEN];  // kalo pointer aja takut error
  char argv[10][MAXIMUM_CMD_LEN];

  char hist[HIST_SIZE][10 * MAXIMUM_CMD_LEN];

  char username[11], cwdName[14], promptHead[3], prompt[27], atSymb[2];
  char cwdIdx = 0xFF;

  int argc, histc = 0, i;

  strncpy(username, "pushOS", 11);
  atSymb[0] = '@';
  atSymb[1] = 0;
  cwdName[0] = '/';
  cwdName[1] = 0;
  promptHead[0] = '>';
  promptHead[1] = ' ';
  promptHead[2] = 0;  // default prompt: "pushOS@/> "

  while (true) {
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
      // TODO: bad UX because doesn't tell the error
      interrupt(0x21, 0, "Terjadi kesalahan saat membaca perintah\n", 0, 0);
      continue;
    }

    // eksekusi perintah
    if (strncmp("shell_cd", argv[0], MAXIMUM_CMD_LEN) == 0) {
      if (argc != 2) {
        interrupt(0x21, 0, "Penggunaan: shell_cd <path/direktori>\n", 0, 0);
      } else {
        shell_cd(&cwdIdx, argv[1], cwdName);
      }
    } else if (strncmp("ls", argv[0], MAXIMUM_CMD_LEN) == 0) {
      shell_ls(cwdIdx);
    } else if (strncmp("cat", argv[0], MAXIMUM_CMD_LEN) == 0) {
      if (argc != 2) {
        interrupt(0x21, 0, "Penggunaan: cat <path/file>\n", 0, 0);
      } else {
        shell_cat(cwdIdx, argv[1]);
      }
    } else if (strncmp("ln", argv[0], MAXIMUM_CMD_LEN) == 0) {
      if (argc != 3) {
        interrupt(0x21, 0, "Penggunaan: ln <path/sumber> <path/tujuan>\n", 0, 0);
      } else {
        shell_hardLink(cwdIdx, argv[1], argv[2]);
      }
    } else if (strncmp("cwd", argv[0], MAXIMUM_CMD_LEN) == 0) {
      printNumber(cwdIdx);
      printString(" - ");
      printString(cwdName);
      printString("\n");
    } else if (strncmp("history", argv[0], MAXIMUM_CMD_LEN) == 0) {
      for (i = 0; i < HIST_SIZE; i++) {
        if (strlen(hist[i]) != 0) {
          printString(hist[i]);
          printString("\n");
        }
      }
    } else if (strncmp("cp", argv[0], MAXIMUM_CMD_LEN) == 0) {
      if (argc != 3) {
        interrupt(0x21, 0,
                  "Penggunaan: cp <path/sumber> <path/tujuan>\n", 0, 0);
      } else {
        shell_cp(cwdIdx, argv[1], argv[2]);
      }
    } else {
      interrupt(0x21, 0, "Perintah ", 0, 0);
      interrupt(0x21, 0, argv[0], 0, 0);
      interrupt(0x21, 0, " tidak dikenali.\n", 0, 0);
    }

    // HISTORY
    histc = (histc >= HIST_SIZE) ? 0 : histc;
    for (i = 1; i < HIST_SIZE; i++) {
        strcpy(hist[i - 1], hist[i]);
    }
    strcpy(hist[HIST_SIZE - 1], command);
    histc++;
  }
}

int commandParser(char *cmd, char *argument) {
  int i, j;
  bool stop = false;

  i = 0, j = 0;
  for (; *cmd == ' '; cmd++)
    ;
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
          interrupt(0x21, 0, path, 0, 0);
          interrupt(0x21, 0, " bukan direktori.\n", 0, 0);
      }
    } else {
      interrupt(0x21, 0, "Direktori ", 0, 0);
      interrupt(0x21, 0, path, 0, 0);
      interrupt(0x21, 0, " tidak ditemukan.\n", 0, 0);
    }
  }
  return;
}

// TODO: ls ke directory lain
void shell_ls(char parentIndex) {
  int i = 0;
  char dir[2 * SECTOR_SIZE];

  interrupt(0x21, 0x0002, dir, 0x101, 0);  // readSector
  interrupt(0x21, 0x0002, dir + 512, 0x102, 0);

  while (i < 1024) {
    if (*(dir + i) == parentIndex && *(dir + i + 2) != 0) {
        interrupt(0x21, 0, dir + i + 2, 0, 0);
        if (*(dir + i + 1) == '\xFF') interrupt(0x21, 0, "/", 0, 0);
        interrupt(0x21, 0, "\n", 0, 0);
    }
    i += 16;
  }
}