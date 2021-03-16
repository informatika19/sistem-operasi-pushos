#include "kernel.h"
#include "lib/math.h"
#include "lib/string.h"

#define FILE_NAME_LENGTH    14
#define FILE_ENTRY_LENGTH   16
#define FILE_ENTRY_TOTAL    64

#define SECTOR_SIZE         512
#define SECTOR_ENTRY_LENGTH 16
#define SECTOR_FILE_TOTAL   32
#define SECTOR_TOTAL        2880

#define MAP_SECTOR          0x100
#define ROOT_SECTOR         0x101
#define SECTORS_SECTOR      0x103

#define ADDR_MAP            MAP_SECTOR * SECTOR_SIZE
#define ADDR_FILES          ROOT_SECTOR * SECTOR_SIZE
#define ADDR_SECTORS        SECTORS_SECTOR * SECTOR_SIZE

#define BLUE 0x9
#define YELLOW 0xE
#define WHITE 0xF

int main() {
  char buffer[SECTOR_SIZE * 2];
  char test[16];
  int *res;
  interrupt(0x10, 0x0013, 0, 0, 0);
  // drawImage();
  printLogoGrafik();
  interrupt(0x15, 0x8600, 0, 5, 0);
  interrupt(0x10, 0x0003, 0, 0, 0);
  printLogoASCII();

  printString("Masukkan input: ");
  readString(test);
  printString("\r\nOutput: ");
  printString(test);
  printString("\r\n");

  strncpy(buffer, test, 16);
  writeFile(test, "user", 1, 0xFF);
  writeFile(buffer, "./test.txt", 1, 0xFF);
  clear(test,16);
  readFile(test, "test.txt", &res, 0xFF);
  printString(test);

  while (1);
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
  char AL, AH; 
  AL = (char) (AX); 
  AH = (char) (AX >> 8); 
  switch (AX) {
    case 0x00:
      printString(BX);
      break;
    case 0x01:
      readString(BX);
      break;
    case 0x02: 
      readSector(BX, CX); 
      break; 
    case 0x03: 
      writeSector(BX, CX); 
      break; 
    case 0x04: 
      readFile(BX, CX, DX, AH); 
      break; 
    case 0x05: 
      writeFile(BX, CX, DX, AH); 
      break;
    default:
      printString("Invalid interrupt");
  }
}

void printString(char *string) {
  while (*string != '\0') {
    interrupt(0x10, 0x0E00 + *string, 0, 0, 0);
    string++;
  }
}

void readString(char *string) {
  int i = 0;
  while (1) {
    int key = interrupt(0x16, 0, 0, 0, 0);
    if (key == 0xD) {              
      string[i] = 0x0;
      return;
    } else if (key == 0x8) {
      if (i > 0) {
        string[i] = 0x0;
        i--;
        interrupt(0x10, 0x0E00 + 0x8, 0, 0, 0);
        i++;
        interrupt(0x10, 0x0E00 + 0x0, 0, 0, 0);
        i--;
        interrupt(0x10, 0x0E00 + 0x8, 0, 0, 0);
      }
    } else {
      string[i] = key;
      interrupt(0x10, 0x0E00 + key, 0, 0, 0);
      i++;
    }     
  }
}

void clear(char *buffer, int length) {
  int i;
  for (i = 0; i < length; i++) {
    buffer[i] = 0;
  }
}

void printLogoGrafik() {
  lineLogo("                                                                               ",0);
  lineLogo("                                                                               ",1);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",2);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",3);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",4);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`           ",5);
  lineLogo("           ..............` `....     `.... `.........` ....    `....           ",6);
  lineLogo("           ..............` `....     `.... `.........` ....    `....           ",7);
  lineLogo("           .....`````..... `....     `.... ....`  ```  ....    `....           ",8);
  lineLogo("           .....`````..... `....     `.... ....`  ```  ....    `....           ",9);
  lineLogo("           ..--`     `.... `....     ..... .....``     ....``` `....           ",10);
  lineLogo("           ..--`     `.... `....     ..... .....``     ....``` `....           ",11);
  lineLogo("           //ooo+:-...-:////. -:-..     :o/:. `../+oo/.   ..-/+oo+:-           ",12);
  lineLogo("           //ooo+:-...-:////. -:-..     :o/:. `../+oo/.   ..-/+oo+:-           ",13);
  lineLogo("           +ooo+//:-:::.`  /oo+:     /ooo/   `-/oooo+``+ooooooooo+//           ",14);
  lineLogo("           +ooo+//:-:::.`  /oo+:     /ooo/   `-/oooo+``+ooooooooo+//           ",15);
  lineLogo("           +ooo:  ``````.  -ooo+`   `/ooo:      .+ooo:`oooo.```-ooo+           ",16);
  lineLogo("           +ooo:  ``````.  -ooo+`   `/ooo:      .+ooo:`oooo.```-ooo+           ",17);
  lineLogo("           +ooo:  -------.  /ooo+:-:+ooo/``/+/:-:+ooo.`ooo+`   -ooo+           ",18);
  lineLogo("           +ooo:  -------.  /ooo+:-:+ooo/``/+/:-:+ooo.`ooo+`   -ooo+           ",19);
  lineLogo("           /ooo-  +++++:.-. `-+ooooooo+-` `+ooooooo+- `ooo+`   .ooo/           ",20);
  lineLogo("           /ooo-  +++++:.-. `-+ooooooo+-` `+ooooooo+- `ooo+`   .ooo/           ",21);
  lineLogo("           `-:.  .++++++:--.` ``.-:-.`` `` ``.-::-.` ` `--` `.  .--`           ",22);
  lineLogo("           `-:.  .++++++:--.` ``.-:-.`` `` ``.-::-.` ` `--` `.  .--`           ",23);
  lineLogo("                                                                               ",24);
  lineLogo("                                                                               ",25);
}

void printLogoASCII() {
  printString("                     _____ _____ _____ _____    _____ _____ \r\n");
  printString("                    |  _  |  |  |   __|  |  |  |     |   __|\r\n");
  printString("                    |   __|  |  |__   |     |  |  |  |__   |\r\n");
  printString("                    |__|  |_____|_____|__|__|  |_____|_____|\r\n");
}

void lineLogo(char *string, int y) {
  int x = 0;
  while (string[x] != '\0') {
    x++;
    switch (string[x]) {
      case 'o':
        interrupt(0x10, 0x0C00 + YELLOW, 0, x, y);
        break;
      case '.':
        interrupt(0x10, 0x0C00 + BLUE, 0, x, y);
        break;
      default:
        interrupt(0x10, 0x0C00 + WHITE, 0, x, y);
    }
  }
}

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
  interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

char *parsePath(char *path, char parentIndex) {
  // path paling banyak 64 ganti dir
  char pathDir[FILE_ENTRY_TOTAL * FILE_NAME_LENGTH];
  int i, j, k;
  i = 0;
  k = 0;
  while (path[i] != '\0') {
    j = 0;

    if (path[i] != '/') {
      while (path[i] != '/') {
        i++;
        j++;
      }

      if (j-1 > FILE_NAME_LENGTH) {
        printString("Path tidak valid!\r\n");
        pathDir[0] = -1;
        return pathDir;
      }

      strncpy(pathDir[k * FILE_ENTRY_LENGTH + 2], path[i], j);
      k++;
    } else {
      i++;
    }
  }

  return pathDir;
}

int isPathValid(char *path, char *parentIndex, char *dirBuffer) {
  char dir[FILE_ENTRY_TOTAL * FILE_NAME_LENGTH];
  char name[FILE_NAME_LENGTH];
  char currentParent;
  int index;
  int i;

  strncpy(dir, parsePath(path, parentIndex), FILE_ENTRY_TOTAL * FILE_NAME_LENGTH);
  if (dir[0] == -1) { return 0; }
  currentParent = parentIndex;

  for (i = 0; i < FILE_ENTRY_TOTAL && dir[i * FILE_ENTRY_LENGTH + 2] != 0x00; i++) {
    clear(name, FILE_NAME_LENGTH);
    if (strncmp(dir[i * FILE_ENTRY_LENGTH + 2], "..", 2) == 0) {
      if (currentParent == 0xFF) {
        return 0;
      } else {
        currentParent = dirBuffer[currentParent * FILE_ENTRY_LENGTH];
        continue;
      }
    } else if (dir[i] == '.') {
      continue;
    }

    strncpy(name, dir[i * FILE_ENTRY_LENGTH + 2], FILE_NAME_LENGTH);
    index = getFileIdx(name, currentParent, dirBuffer);
    if (index == -1) { return 0; }
    currentParent = dirBuffer[index * FILE_ENTRY_LENGTH];
  }

  *parentIndex = currentParent;
  return 1;
}


int getFileIdx(char *name, char parentIndex, char *dirBuffer) {
  int idx = 0;
  while (idx < FILE_ENTRY_TOTAL) {
    if (strncmp(name, dirBuffer[idx * FILE_ENTRY_LENGTH + 2], strlen(name)) == 0
      && dirBuffer[idx * FILE_ENTRY_LENGTH] == parentIndex) return idx;
    idx++;
  }
  return -1;
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
  char mapBuffer[SECTOR_SIZE];
  char dirBuffer[SECTOR_SIZE*2];
  char secBuffer[SECTOR_SIZE];
  char *parentIdx;
  char fName[FILE_NAME_LENGTH];
  char fileIdx;
  int noSector;
  int idxSec;
  int *isFile, fNameLen;

  // baca sektor
  readSector(mapBuffer, MAP_SECTOR);
  readSector(dirBuffer, ROOT_SECTOR);
  readSector(dirBuffer+SECTOR_SIZE, ROOT_SECTOR+1);
  readSector(secBuffer+SECTOR_SIZE, SECTORS_SECTOR);

  // cek apakah namanya sesuai
  strcpy(fName, findFName(path, &isFile));
  fNameLen = strlen(fName);
  if (isFile != 1) {
    // printString("Bukan file\r\n");
    *result = -1;
    return;
  }

  strncpy(&parentIdx, parentIndex, 1);
  if (isPathValid(path, &parentIdx, dirBuffer) == -1) {
    // printString("Path tidak valid");
    // printString("File tidak ditemukan\r\n");
    *result = -1;
    return;
  }

  fileIdx = getFileIdx(fName, parentIdx, dirBuffer);
  idxSec = dirBuffer[SECTOR_ENTRY_LENGTH * fileIdx + 1];
  noSector = 0;

  while (secBuffer[SECTOR_ENTRY_LENGTH * idxSec + noSector] != 0) {
    readSector(buffer + (noSector * SECTOR_SIZE), secBuffer[SECTOR_ENTRY_LENGTH * idxSec + noSector]);
    noSector++;
  }

  *result = 1;
}

/**
 * @param sectors return value:
 * -1 file udah ada,
 * -2 tidak cukup entry,
 * -3 tidak cukup sector kosong,
 * -4 folder tidak valid
 * - harus <= 16 
 */
void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
  int i, j, entry, fNameLen, isFile, indexS, secIndex;
  char mapBuffer[SECTOR_SIZE];
  char dirBuffer[SECTOR_SIZE*2];
  char secBuffer[SECTOR_SIZE];
  char *parentIdx;
  char fName[FILE_NAME_LENGTH];

  // Membaca sectors
  readSector(mapBuffer, MAP_SECTOR);
  readSector(dirBuffer, ROOT_SECTOR);
  readSector(dirBuffer+SECTOR_SIZE, ROOT_SECTOR+1);
  readSector(secBuffer+SECTOR_SIZE, SECTORS_SECTOR);

  // Mem-validasi path
  strncpy(parentIdx, parentIndex, 1);
  if (isPathValid(path, &parentIdx, dirBuffer) == -1) {
    // printString("Path tidak valid");
    *sectors = -4;
    return;
  }

  strcpy(fName, findFName(path, &isFile));
  fNameLen = strlen(fName);

  // Mengecek apakah file sudah ada atau belum
  for (entry = 0; entry < FILE_ENTRY_TOTAL; entry++) {
    if (parentIdx == dirBuffer[FILE_ENTRY_LENGTH * entry]
      && (strncmp(fName, dirBuffer[FILE_ENTRY_LENGTH * entry + 2], FILE_NAME_LENGTH) == 0)) {
      // printString("File dengan nama yang sama sudah ada");
      *sectors = -1;
      return;
    }
  }

  // Mengecek ketersediaan entry
  for (entry = 0; entry < FILE_ENTRY_TOTAL; entry++) {
    if (dirBuffer[FILE_ENTRY_LENGTH * entry + 2] == 0x00) {
      break;
    }
  }

  if (entry == FILE_ENTRY_TOTAL) {
    // printString("Directory penuh\r\n\0");
    *sectors = -2;
    return;
  }

  // Mengecek apakah jumlah sector di map cukup untuk buffer
  if (getMapEmptySectorCount(mapBuffer) < sectors) {
    // printString("Map sector yang kosong tidak mencukupi\r\n");
    *sectors = -3;
    return;
  }

  // Membersihkan sector yang akan digunakan
  clear(dirBuffer[FILE_ENTRY_LENGTH * entry], FILE_ENTRY_LENGTH);

  // Menyimpan parentIndex
  dirBuffer[FILE_ENTRY_LENGTH * entry] = parentIdx;

  // Menyimpan flag S
  if (isFile != 1) {
    dirBuffer[FILE_ENTRY_LENGTH * entry + 1] = 0xFF;
    return;
  }

  indexS = getSectorsEmptyEntry(secBuffer);
  if (indexS == -1) {
    // printString("File penuh\r\n\0");
    *sectors = -3;
    return;
  }
  
  dirBuffer[FILE_ENTRY_LENGTH * entry + 1] = indexS;

  // Menyimpan nama file pada dir, nama harus kurang dari sama dengan 14
  if (fNameLen < FILE_NAME_LENGTH-1) {
    strncpy(dirBuffer[FILE_ENTRY_LENGTH * entry + 2], fName, fNameLen);
    // dirBuffer[FILE_ENTRY_LENGTH * entry + 2 + fNameLen] = '\0';
  }

  // Menulis sector
  for (i = 0; i < sectors; i++) {
    // Mencari sector kosong 
    secIndex = 0;
    while (mapBuffer[secIndex] != 0x0 && secIndex < 256) { secIndex++; } // 256 = half of sector size
    mapBuffer[secIndex] = 0xFF;

    secBuffer[indexS * SECTOR_ENTRY_LENGTH + i] = secIndex;
    writeSector(buffer+(i * SECTOR_SIZE), secIndex);
  }

  writeSector(mapBuffer, MAP_SECTOR);
  writeSector(dirBuffer, ROOT_SECTOR);
  writeSector(dirBuffer+SECTOR_SIZE, ROOT_SECTOR+1);
  writeSector(secBuffer, SECTORS_SECTOR);
}

char *findFName(char *path, int *isFile) {
  int pathLen;
  int i;
  int j;
  char fName[FILE_NAME_LENGTH];

  pathLen = strlen(path);
  *isFile = 0;

  i = pathLen - 1;
  j = 0;
  while (path[i] != '/' && i >= 0) {
    if (path[i] == '.') { *isFile = 1; }
    i--;
    j++;
  }

  strncpy(fName, path[i], j);
  return fName;
}

// char *getFileFromIdx(char idx, char *files) {
//   return files[idx*SECTOR_ENTRY_LENGTH+2];
// }

int getMapEmptySectorCount(char *mapBuffer) {
  int i, count;
  count = 0;
  for (i = 0; i < SECTOR_SIZE; i++)
    if (mapBuffer[i] == 0x00) count++;
  return count;
}

int getSectorsEmptyEntry(char *secBuffer) {
  int i;
  for (i = 0x00; i < SECTOR_FILE_TOTAL; i++)
    if (secBuffer[i] == 0x00)
      return i;
  return -1;
}

void tokenizeCommand(char* raw, char* command, char* param) {
  int i, j, k;
  int haveParam;
  i = 0;
  j = 0;
  k = 0;
  haveParam = 0;

  while (raw[i] != '\0') {
    if (raw[i]==' ') {
      haveParam = 1;
      i++;
    } else if (haveParam == 0) {
      command[j] = raw[i];
      j++;
      i++;
    } else {
      param[k] = raw[i];
      k++;
      i++;
    }
  }
  if (haveParam == 0) {
    param[k] = '\0' ;
  }
}

// char* tokenizeCommand(char* raw, int* commandLen)
// {
//   char* temp;

//   int i = 0;

//   while(*raw != 0x00 && *raw != ' ')
//   {
//     temp[i] == *raw;
//     raw++;
//     i++;
//   }

//   temp[i] = 0x00;

//   *commandLen = i;
//   return temp;

// }

void shell_cd(char** currentDir, char* params, char* dirBuffer) {
  char *fileName;
  char *parentIdx;
  int *isFile;

  strncpy(parentIdx, *currentDir, 14);

  if (params[0] != '\0') {
    if (isPathValid(params, parentIdx, dirBuffer) == 1) {
      fileName = findFName(params, &isFile);
      if (*isFile != 1) {
        *currentDir = getFileIdx(fileName, parentIdx, dirBuffer);
      } else {
        printString("Input tidak valid \r\n");
      }    
    }
  } else {
    printString("Input tidak valid \r\n");
  }
}

void shell_ls(char** currentDir, char* params, char* dirBuffer) {
  char *finalName;
  char *parentIdx;
  int idx;
  int isFile;

  strncpy(parentIdx, *currentDir, 14);

  if (isPathValid(params, parentIdx, dirBuffer) == 1) {
  strncpy(parentIdx, *currentDir, FILE_NAME_LENGTH);

  if (params[0] == '\0') {
    for (idx = 0; idx < SECTOR_FILE_TOTAL; idx++) {
      if (dirBuffer[idx * SECTOR_SIZE] == parentIdx) {
        printString(getFileFromIdx(idx, dirBuffer));
        printString("\r\n");
      }
    }   
  } else if (isPathValid(params, parentIdx, dirBuffer) == 1) {
    for (idx = 0; idx < SECTOR_FILE_TOTAL; idx++) {
      if (dirBuffer[idx * SECTOR_SIZE] == parentIdx) {
        printString(getFileFromIdx(idx, dirBuffer));
        printString("\r\n");
      }
    }
  }
}

void shell_cat(char** currentDir, char* params, char* dirBuffer) {
  char *parentIdx;
  char *buffer;
  int *result;

  strncpy(parentIdx, *currentDir, FILE_NAME_LENGTH);

  readFile(buffer, params, result, parentIdx);

  if (*result == 1) {
    printString(buffer);
  } else {
    printString("File tidak ditemukan\r\n");
  }
}

void shell_ln(char** currentDir, char* params) {
  
}

/*
Command list
  cd <dir>
    change directory

  ls
    list directory

  cat <file>
    print contents of file

  ln <file> <second_filename>
    hardlink (create second name for file)
*/

#define shell_isCmd(cmd) (strncmp(command, cmd) == 0;)

void shell() {
  char* rawcommand;
  char* command;
  char* params;
  char* currAbsDir;

  int cmlen;

  while (1) {
    printString(currAbsDir);
    printString("> ");
    readString(rawcommand);

    tokenizeCommand(rawcommand, command, params);
    // params = rawcommand+cmlen;

    if (shell_isCmd("cd")) {
      // change directory
      shell_cd(&currAbsDir, params, dirBuffer);        
    } else if (shell_isCmd("ls")) {
      // list directory
      shell_ls(&currAbsDir, params, dirBuffer);
    } else if (shell_isCmd("cat")) {
      // cat
      shell_cat(&currAbsDir, params);
    } else if (shell_isCmd("ln")) {
      // ln
      shell_ln(&currAbsDir, params);
    } else {
      printString("Invalid command\r\n");
    }
  }
}


