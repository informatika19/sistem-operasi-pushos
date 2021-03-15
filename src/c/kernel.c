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
  char buffer[SECTOR_SIZE * SECTOR_ENTRY_LENGTH];
  char test[16];
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
  // newline(*row);
  clear(test,16);

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

      if (j > FILE_NAME_LENGTH) {
        printString("Path tidak valid!\r\n");
        return -1;
      }

      strncpy(pathDir[k * FILE_ENTRY_LENGTH + 2], path[j], j);
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
  strncpy(dir, parsePath(path, parentIndex), FILE_ENTRY_TOTAL * FILE_NAME_LENGTH); // bruh
  currentParent = parentIndex;
  for (i = 0; i < FILE_ENTRY_TOTAL && dir[i * FILE_ENTRY_LENGTH + 2] != 0x00; i++) {
    clear(name, FILE_NAME_LENGTH);
    if (strncmp(dir[i * FILE_ENTRY_LENGTH], "..", 2) == 0) {
      if (currentParent == 0xFF) {
        return 0;
      } else {
        currentParent = dirBuffer[currentParent * FILE_ENTRY_LENGTH];
        continue;
      }
    } else if (dir[i] == '.') {
      continue;
    }
    // kalo turun itu compare nama dan parent
    strncpy(name, dir[i + 2], FILE_NAME_LENGTH);
    index = getFileIdx(name, currentParent, dirBuffer);
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
  int i, j, entry, fNameLen, sectorsWritten, isFile, indexS, secIndex;
  char mapBuffer[SECTOR_SIZE];
  char dirBuffer[SECTOR_SIZE*2];
  char secBuffer[SECTOR_SIZE];
  char parentIdx[1];
  char fName[FILE_NAME_LENGTH];
  char subBuff[SECTOR_SIZE];
  char sectorPointer[SECTOR_ENTRY_LENGTH];

  // Membaca sectors
  readSector(mapBuffer, MAP_SECTOR);
  readSector(dirBuffer, ROOT_SECTOR);
  readSector(dirBuffer+SECTOR_SIZE, ROOT_SECTOR+1);
  readSector(secBuffer+SECTOR_SIZE, SECTORS_SECTOR);

  // Mem-validasi path
  strncpy(parentIdx, parentIndex, 1);
  if (isPathValid(path, parentIdx, dirBuffer) == -1) {
    // printString("Path tidak valid");
    *sectors = -4;
    return;
  }

  strcpy(fName, findFName(path, isFile));
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
  dirBuffer[FILE_ENTRY_LENGTH * entry + 1] = parentIdx;

  // Menyimpan flag S
  if (isFile != 1) {
    dirBuffer[FILE_ENTRY_LENGTH * entry + 2] = 0xFF;
    return;
  }

  indexS = getSectorsEmptyEntry(secBuffer);
  if (indexS == -1) {
    // printString("File penuh\r\n\0");
    *sectors = -3;
    return;
  }
  
  dirBuffer[FILE_ENTRY_LENGTH * entry + 2] = indexS;

  // Menyimpan nama file pada dir, nama harus kurang dari sama dengan 14
  if (fNameLen < FILE_NAME_LENGTH) {
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
    writeSector(buffer+(SECTOR_SIZE * i), secIndex);
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
