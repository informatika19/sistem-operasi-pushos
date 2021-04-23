#include "headers/folderIO.h"

#include "headers/string.h"
#include "headers/boolean.h"
#include "headers/math.h"

void createFolder(char parentIndex, char* folderName) {
  int i, j, entry, sectorNeeded, sectorFree = 0, sectorsToUse[16], entrySectors;
  bool alreadyExists = false, parentExists = (parentIndex == '\xFF');
  char map[SECTOR_SIZE], dir[2 * SECTOR_SIZE], sec[SECTOR_SIZE];
  char fileName[FILE_NAME_LENGTH], parents[FILE_ENTRY_TOTAL][FILE_NAME_LENGTH];

  readSector(map, MAP_SECTOR);
  readSector(dir, ROOT_SECTOR);
  readSector(dir + SECTOR_SIZE, ROOT_SECTOR+1);
  readSector(sec, SECTORS_SECTOR);

  // ngecek file dengan yang sama di parent index yang sama udah ada atau
  // belum

  i = 0;
  while (i < 2 * SECTOR_SIZE && !alreadyExists) {
    parentExists = !parentExists ? *(dir + i) == parentIndex : parentExists;
    alreadyExists = *(dir + i) == parentIndex &&
                    strncmp(dir + i + 2, fileName, FILE_NAME_LENGTH) == 0;
    i += 0x10;
  }
  if (alreadyExists) {
    printString("Folder name already taken\r\n");
    return;
  }
  if (!parentExists) {
    printString("Invalid parent directory\r\n");
    printNumber(parentIndex);
    return;
  }

  // nyari entri kosong di sektor files yang kosong kosong
  entry = 0;
  for (; *(dir + entry + 2) && entry < 2 * SECTOR_SIZE; entry += 0x10);
  if (entry >= 2 * SECTOR_SIZE) {
    printString("Directory space full\r\n");
    return;
  }

  // akusisi entry yang ditemukan sebelumnya
  *(dir + entry) = parentIndex;
  *(dir + entry + 1) = 0xFF;
  strncpy(dir + entry + 2, folderName, FILE_NAME_LENGTH);

  printString("Created folder with name ");
  printString(folderName);
  printString(" with parent directory idx ");
  printNumber(parentIndex);
  printString(" with directory idx ");
  printNumber(entry);


  // tulis perubahan
  writeSector(map, MAP_SECTOR);
  writeSector(dir, ROOT_SECTOR);
  writeSector(dir + SECTOR_SIZE, ROOT_SECTOR+1);
  writeSector(sec, SECTORS_SECTOR);


}