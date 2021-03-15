#include "kernel.h"
#include "lib/math.h"
#include "lib/string.h"

#define FILE_NAME_LENGTH 14
#define FILE_ENTRY_LENGTH 16
#define FILE_ENTRY_TOTAL 64
#define SECTOR_SIZE 512
#define SECTOR_FILE_TOTAL 32
#define SECTOR_TOTAL 2880
#define MAP_SECTOR 0x100
#define ROOT_SECTOR 0x101
#define SECTORS_SECTOR 0x103

#define BLUE 0x9
#define YELLOW 0xE
#define WHITE 0xF

int main() {
  char test[16];
  interrupt(0x10, 0x0013, 0, 0, 0);
  // drawImage();
  printLogoGrafik();
  interrupt(0x15, 0x8600, 0, 5, 0);
  interrupt(0x10, 0x0003, 0, 0, 0);
  printLogoASCII();
  printString("Masukkan input: ");
  readString(test);
  printString(test);
  clear(test,16);
  while (1);
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
  char AL, AH; 
  AL = (char) (AX); 
  AH = (char) (AX >> 8); 
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
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
  while(i < 16) {
    *string = interrupt(0x16, 0, 0, 0, 0);
    
    if (*string != '\r') {
      string++;
      i++;
    } else {
      *string = '\0';
      i = 16;
    }
  }
}

void readString(char *string){
  int i = 0;
  while (1) {
    int key = interrupt(0x16, 0, 0, 0, 0);
    if (key == 0xD) {              
      string[i] = 0xA;
      string[i+1] = 0x0;
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

void newLine(int *row) {
  interrupt(0x10, 0x0002, 0, 0, *row * 0x0100);
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

void readFile(char *buffer, char *path, int *result, char parentIndex) {

}

/**
 * @param sectors return value:
 * -1 file udah ada,
 * -2 tidak cukup entry,
 * -3 tidak cukup sector kosong,
 * -4 folder tidak valid
 */
void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
  int i, j, entry, fileNameLen, sectorsWritten;
  char mapBuffer[SECTOR_SIZE];
  char dirBuffer[SECTOR_SIZE*2];
  char secBuffer[SECTOR_SIZE];
  char fileName[FILE_NAME_LENGTH];

  // Membaca sectors
  readSector(mapBuffer, MAP_SECTOR);
  readSector(dirBuffer, ROOT_SECTOR);
  readSector(dirBuffer+SECTOR_SIZE, ROOT_SECTOR+1);
  readSector(secBuffer+SECTOR_SIZE, SECTORS_SECTOR); // what for?

  if ((dirBuffer[(FILE_ENTRY_LENGTH * parentIndex) + 1] != 0xFF) && (parentIndex != 0xFF)) {
    // printString("Folder tidak valid");
    *sectors = -4;
    return;
  }

  strcpy(fileName, parsePath(path));
  fileNameLen = strlen(fileName);

  // Mengecek apakah file sudah ada atau belum
  for (entry = 0; entry < FILE_ENTRY_TOTAL; entry++) {
    if (parentIndex == dirBuffer[FILE_ENTRY_LENGTH * entry] && (strncmp(fileName, dirBuffer[FILE_ENTRY_LENGTH * entry + 2], FILE_NAME_LENGTH) == 0)) {
      // printString("File dengan nama yang sama sudah ada");
      *sectors = -1;
      return;
    }
  }

  // Mengecek ketersediaan entry
  for (entry = 0; entry < FILE_ENTRY_TOTAL; entry++) {
    if (dirBuffer[FILE_ENTRY_LENGTH * entry] == 0x00) {
      break;
    }
  }

  if (entry == FILE_ENTRY_TOTAL) {
    // printString("File penuh\r\n\0");
    *sectors = -2;
    return;
  }

  // Mengecek apakah jumlah sector di map cukup untuk buffer
  if (getMapEmptySector(mapBuffer) < sectors) {
    // printString("Map sector yang kosong tidak mencukupi\r\n");
    *sectors = -3;
    return;
  }

  // Membersihkan sector yang akan digunakan
  clear(dirBuffer[FILE_ENTRY_LENGTH * entry], FILE_ENTRY_LENGTH);

  // Menyimpan nama file pada dir, nama harus kurang dari sama dengan 14
  if (fileNameLen < FILE_NAME_LENGTH) {
    strncpy(dirBuffer[FILE_ENTRY_LENGTH * entry + 2], fileName, fileNameLen);
    for (i = fileNameLen; i < FILE_NAME_LENGTH; i++) {
      dirBuffer[FILE_ENTRY_LENGTH * entry + 2 + i] = 0x0;
    }
  }

  // Menulis sector
  for (i = 0, i < sectors; i++) {


  }

  writeSector(mapBuffer, MAP_SECTOR);
  writeSector(dirBuffer, ROOT_SECTOR);
  writeSector(dirBuffer, ROOT_SECTOR+1);
  writeSector(secBuffer, SECTORS_SECTOR);
}

char *findFileName(char *path) {
  // buat caari nama, klo kurang dari 14 isi 0x0
}

int getMapEmptySector(char *mapBuffer) {
  int i, count;
  count = 0;
  for (i = 0; i < SECTOR_SIZE; i++)
    if (mapBuffer[i] == 0x00) count++;
  return count;
}
