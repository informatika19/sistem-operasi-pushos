#include "headers/fileIO.h"
#include "headers/boolean.h"

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
  interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
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
  findFName(path, &isFile, &fName);
  fNameLen = strlen(fName);
  if (isFile != 1) {
    // printString("Bukan file\r\n");
    *result = -1;
    return;
  }

  strncpy(&parentIdx, parentIndex, 1);
  if (isPathValid(path, &parentIdx, dirBuffer) == 0) {
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
 */

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
  int i, j, entry, fNameLen, isFile, flagS, secsNeeded, secIndex;
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
  strncpy(&parentIdx, parentIndex, 1);
  if (isPathValid(path, &parentIdx, dirBuffer) == 0) {
    // printString("Path tidak valid");
    *sectors = -4;
    return;
  }

  findFName(path, &isFile, &fName);
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

  // Mencari jumlah sector yang diperlukan
  secsNeeded = 0;
  while (buffer[SECTOR_SIZE * secsNeeded] != 0x00) {
    secsNeeded++;
  }

  // Mengecek apakah jumlah sector di map cukup untuk buffer
  if (getMapEmptySectorCount(mapBuffer) < secsNeeded) {
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

  flagS = getSectorsEmptyEntry(secBuffer);
  if (flagS == -1) {
    // printString("File penuh\r\n\0");
    *sectors = -3;
    return;
  }
  
  dirBuffer[FILE_ENTRY_LENGTH * entry + 1] = flagS;

  // Menyimpan nama file pada dir, nama harus kurang dari sama dengan 14
  if (fNameLen < FILE_NAME_LENGTH-1) {
    strncpy(dirBuffer[FILE_ENTRY_LENGTH * entry + 2], fName, fNameLen);
    // dirBuffer[FILE_ENTRY_LENGTH * entry + 2 + fNameLen] = '\0';
  }

  // Menulis sector
  for (i = 0; i < secsNeeded; i++) {
    // Mencari sector kosong 
    secIndex = 0;
    while (mapBuffer[secIndex] != 0x0 && secIndex < 256) { secIndex++; } // 256 = half of sector size
    mapBuffer[secIndex] = 0xFF;

    secBuffer[flagS * SECTOR_ENTRY_LENGTH + i] = secIndex;
    writeSector(buffer+(i * SECTOR_SIZE), secIndex);
  }

  *sectors = flagS;

  writeSector(mapBuffer, MAP_SECTOR);
  writeSector(dirBuffer, ROOT_SECTOR);
  writeSector(dirBuffer+SECTOR_SIZE, ROOT_SECTOR+1);
  writeSector(secBuffer, SECTORS_SECTOR);
}

void parsePath(char *path, char *parsedPath[]) {
  // char pathDir[FILE_ENTRY_TOTAL * FILE_NAME_LENGTH];
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
        *parsedPath[0] = -1;
        return;
      }

      strncpy(*parsedPath[k * FILE_ENTRY_LENGTH + 2], path[i], j);
      k++;
    } else {
      i++;
    }
  }
}

int isPathValid(char *path, char *parentIndex, char *dirBuffer) {
  char dir[MAX_CD * FILE_NAME_LENGTH];
  char name[FILE_NAME_LENGTH];
  char currentParent;
  int index;
  int i;

  parsePath(path, &dir);
  if (dir[0] == -1) { return 0; }
  currentParent = parentIndex;

  for (i = 0; i < MAX_CD && dir[i * FILE_ENTRY_LENGTH + 2] != 0x00; i++) {
    clear(name, FILE_NAME_LENGTH);
    if (strncmp(dir[i * FILE_ENTRY_LENGTH + 2], "..", 2) == 0) {
      if (currentParent == 0xFF) {
        return 0;
      } else {
        currentParent = dirBuffer[currentParent * FILE_ENTRY_LENGTH];
        continue;
      }
    } else if (dir[i * FILE_ENTRY_LENGTH + 2] == '.') {
      continue;
    }

    strncpy(name, dir[i * FILE_ENTRY_LENGTH + 2], FILE_NAME_LENGTH);
    index = getFileIdx(name, currentParent, dirBuffer);
    if (index == -1) { return 0; }
    currentParent = dirBuffer[index * FILE_ENTRY_LENGTH];
  }

  strncpy(&parentIndex, currentParent, 1);
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

void findFName(char *path, int *isFile, char *fName) {
  int pathLen;
  int i;
  int j;

  pathLen = strlen(path);
  *isFile = 0;

  i = pathLen - 1;
  j = 0;
  while (path[i] != '/' && i >= 0) {
    if (path[i] == '.') { *isFile = 1; }
    i--;
    j++;
  }

  strncpy(&fName, path[i], j);
}

int getIdxOfFileWithNameAndParent(char* name, int parentIdx) {
  int found = -1;
  int i = 0;
  char dirBuffer[SECTOR_SIZE*2];
  int entry;
  char* fName;
  strcpy(fName, name);
  readSector(dirBuffer, ROOT_SECTOR);

    // Mengecek apakah file sudah ada atau belum
  for (entry = 0; entry < FILE_ENTRY_TOTAL; entry++) {
    if (parentIdx == dirBuffer[FILE_ENTRY_LENGTH * entry]
      && (strncmp(fName, dirBuffer[FILE_ENTRY_LENGTH * entry + 2], FILE_NAME_LENGTH) == 0)) {
      return entry;
    }
  }

  // while (found == -1)
  // {
  //   if (dirBuffer[i*FILE_ENTRY_LENGTH] == parentIdx)
  //   {
  //     if (strcmp(name, dirBuffer[i*FILE_ENTRY_LENGTH+2], 14) == 0)
  //     {
  //       return dirBuffer[i*FILE_ENTRY_LENGTH+1];
  //     }
  //   }
  // }

  return -1;
}

int getNameOfFileWithIdx(int idX, char* name) {
  char dirBuffer[SECTOR_SIZE*2];
  readSector(dirBuffer, ROOT_SECTOR);
  strncpy(name, dirBuffer[FILE_ENTRY_LENGTH * idX + 2], FILE_NAME_LENGTH);

  return strlen(name);
}

char* getFileFromIdx(char idx, char *files) {
  char fName[FILE_ENTRY_LENGTH];
  strncpy(fName, files[idx * FILE_ENTRY_LENGTH + 2], FILE_NAME_LENGTH);
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

int getFileIndex(char *path, char parentIndex, char *dir) {
  char *entry;
  char tmpP[64][14], fname[14];
  char parents[64][14];
  bool found;
  int tmp = SECTOR_SIZE * 2;
  int i, j, jmlParents;

  // append fname di array tmpP
  jmlParents = parsePath1(path, tmpP, fname);
  strcpy(tmpP[jmlParents], fname);
  jmlParents++;

  // Untuk handle . (dihapus)
  i = 0, j = 0;
  for (; i < jmlParents; i++) {
    if (strncmp(tmpP[i], ".", 2) == 0) {
      // do nothing
    } else {
      strcpy(parents[j], tmpP[i]);
      j++;
    }
  }

  jmlParents = j;
  j = 0;
  found = true;
  while (j < jmlParents && found) {
    found = false;
    // iterasi dalam file buat nyari yang parentIndexnya sesuai
    //      kalo ketemu, cari yang namanya sama strncmp(entry+2,...)
    //          kalo namanya ga sama, found = true
    if (strncmp(parents[j], "..", 14) == 0) {
      found = true;  // kasus .. sebagai elemen terakhir di parents
      if (parentIndex != '\xFF') {
        parentIndex = *(dir + (parentIndex * 0x10)) & 0xFF;
      }
      // kalo di root do nothing
    } else {
      i = 0;
      while (!found && i < tmp) {
        entry = dir + i;
        found = *entry == parentIndex
                    ? strncmp(entry + 2, parents[j], 14) == 0
                    : found;
        i += 0x10;
      }
      // kalo gaada file yang parentIndex dan namanya sesuai di path
      // (bakal terminate loop) ganti parentIndex jadi indeks dari
      // file/folder yang sesuai kriteria atas
      if (found) parentIndex = (i / 0x10) - 1;
    }
    j++;
  }

  return !found ? -1 : parentIndex;
}

int parsePath1(char *path, char *parents, char *fname) {
  int i, j;

  path = path + (1 * (*path == '/'));
  j = strlen(path) - 1;
  *(path + j) = *(path + j) * (*(path + j) != '/');

  i = 0, j = 0;
  while (*path != '\0') {
    switch (*path) {
      case '/':
        *(parents + j + i) = 0;
        j += 14 * (i != 0);
        i = 0;
        break;
      default:
        *(parents + j + i) = *path;
        i++;
    }
    path++;
  }

  *(parents + j + i) = 0;
  strncpy(fname, parents + j, 14);

  return div(j, 14);
}
