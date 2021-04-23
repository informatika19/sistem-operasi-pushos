#include "headers/folderIO.h"
#include "../lib/lib.h"


/*
1. get parameter nama folder apa yg mau dihapus
2. akses ke folder nya
3. traversal seluruh file dalam folder & hapus
4. kalau ketemu folder di dalam folder, error aja
*/

void deleteFolder(char *path, char *parentIndex){
  char cwdIdx;
  char argv[MAXIMUM_ARGC][MAXIMUM_CMD_LEN], cwdName[FILE_NAME_LENGTH];
  char buf[SECTOR_ENTRY_LENGTH * SECTOR_SIZE], path[MAXIMUM_CMD_LEN];
  char map[SECTOR_SIZE], dir[2 * SECTOR_SIZE], sec[SECTOR_SIZE];
  int res = 0, success, i, res2;
  int isFolderFound=0;
  char parent;

  readSector(map, MAP_SECTOR);
  readSector(dir, ROOT_SECTOR);
  readSector(dir+SECTOR_TOTAL, ROOT_SECTOR+1);
  readSector(sec, SECTORS_SECTOR);

  //1
  getParameter(&cwdIdx, cwdName, argv, &success);

  //2
  parent = getFileIndex(argv[1], cwdIdx, dir);
  
  //3
  i = 0;
  while (i < 2 * 512) {
    if (*(dir + i) == parent && *(dir + i + 2) != 0) {
      if (*(dir + i + 1) == '\xFF'){
        isFolderFound = 1;
        break;
      }
    }
    i += 16;
  }

  if (isFolderFound == 0){
    i = 0;
    while (i < 2 * 512) {
      if (*(dir + i) == parent && *(dir + i + 2) != 0) {
        int res;
        removeFile(dir + i + 2, &res, parent);
      }
      i += 16;
    }

    removeFile(argv[1], &res2, cwdIdx);
    printString("Berhasil dihapus");
  }

  //4
  else if (isFolderFound == 1){
    printString("Gagal menghapus folder");
  } 
}

#include "headers/string.h"
#include "headers/boolean.h"
#include "headers/math.h"

/*
change this to work on folders
*/
void createFolder(char *foldername, char parentIndex) {
  int i, j, entry, fNameLen, isFile, flagS, secsNeeded, secIndex;
  char mapBuffer[SECTOR_SIZE];
  char dirBuffer[SECTOR_SIZE*2];
  char secBuffer[SECTOR_SIZE];
  char fName[FILE_NAME_LENGTH];

  // Membaca sectors
  readSector(mapBuffer, MAP_SECTOR);
  readSector(dirBuffer, ROOT_SECTOR);
  readSector(dirBuffer+SECTOR_SIZE, ROOT_SECTOR+1);
  readSector(secBuffer+SECTOR_SIZE, SECTORS_SECTOR);

  // Mem-validasi path
//   strncpy(&parentIdx, parentIndex, 1);
//   if (isPathValid(path, &parentIdx, dirBuffer) == 0) {
//     // printString("Path tidak valid");
//     *sectors = -4;
//     return;
//   }

//   findFName(path, &isFile, &fName);
  fNameLen = strlen(foldername);

  // Mengecek apakah file sudah ada atau belum
  for (entry = 0; entry < FILE_ENTRY_TOTAL; entry++) {
    if (parentIndex == dirBuffer[FILE_ENTRY_LENGTH * entry]
      && (strncmp(fName, dirBuffer[FILE_ENTRY_LENGTH * entry + 2], FILE_NAME_LENGTH) == 0)) {
      printString("File dengan nama yang sama sudah ada");
    //   *sectors = -1;
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
    printString("Directory penuh\r\n\0");
    // *sectors = -2;
    return;
  }

  // Mencari jumlah sector yang diperlukan // not needed
//   secsNeeded = 0;
//   while (buffer[SECTOR_SIZE * secsNeeded] != 0x00) {
//     secsNeeded++;
//   }

  // Mengecek apakah jumlah sector di map cukup untuk buffer // not needed
//   if (getMapEmptySectorCount(mapBuffer) < secsNeeded) {
//     // printString("Map sector yang kosong tidak mencukupi\r\n");
//     *sectors = -3;
//     return;
//   }

  // Membersihkan sector yang akan digunakan  // not needed
  clear(dirBuffer[FILE_ENTRY_LENGTH * entry], FILE_ENTRY_LENGTH);

  // Menyimpan parentIndex
  dirBuffer[FILE_ENTRY_LENGTH * entry] = parentIndex;

//   // Menyimpan flag S // is always true
    // Jika file (bukan folder)
  if (isFile == 1) {
    return;
  }

//   flagS = getSectorsEmptyEntry(secBuffer); // not needed
//   if (flagS == -1) {
//     // printString("File penuh\r\n\0");
//     *sectors = -3;
//     return;
//   }
  
  dirBuffer[FILE_ENTRY_LENGTH * entry + 1] = 0xFF;

  // Menyimpan nama file pada dir, nama harus kurang dari sama dengan 14
  if (fNameLen < FILE_NAME_LENGTH-1) {
    strncpy(dirBuffer[FILE_ENTRY_LENGTH * entry + 2], fName, fNameLen);
    // dirBuffer[FILE_ENTRY_LENGTH * entry + 2 + fNameLen] = '\0';
  }

//   // Menulis sector
//   for (i = 0; i < secsNeeded; i++) {
//     // Mencari sector kosong 
//     secIndex = 0;
//     while (mapBuffer[secIndex] != 0x0 && secIndex < 256) { secIndex++; } // 256 = half of sector size
//     mapBuffer[secIndex] = 0xFF;

//     secBuffer[flagS * SECTOR_ENTRY_LENGTH + i] = secIndex;
//     writeSector(buffer+(i * SECTOR_SIZE), secIndex);
//   }

//   *sectors = flagS;

  writeSector(mapBuffer, MAP_SECTOR);
  writeSector(dirBuffer, ROOT_SECTOR);
  writeSector(dirBuffer+SECTOR_SIZE, ROOT_SECTOR+1);
  writeSector(secBuffer, SECTORS_SECTOR);
}
