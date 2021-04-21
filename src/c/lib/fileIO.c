#include "headers/fileIO.h"
#include "headers/string.h"
#include "headers/boolean.h"
#include "headers/math.h"

/**
 * @param sector index sector di system img
 */
void clearSector(int sector) {
  char buffer[SECTOR_SIZE];
  clear(buffer, SECTOR_SIZE);
  interrupt(0x21, 0x0003, buffer, sector, 0); // writeSector
}

void clear(char *buffer, int length) {
  int i;
  for (; i < length; i++) {
    *(buffer + i) = 0;
  }
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
  interrupt(0x21, (parentIndex << 8) + 0x04, buffer, path, result); // readFile
}

void writeFile(char *buffer, char *path, int *sections, char parentIndex) {
  interrupt(0x21, (parentIndex << 8) + 0x05, buffer, path, sections); // writeFile
}

/**
 * @param result
 * -1 file tidak ada
 * -2 path tidak valid
 */
void removeFile(char *path, int *result, char parentIndex) {
  int i, j, sectorsUsed[16], s;
  bool exists = false, parentExists = (parentIndex == '\xFF');
  char map[SECTOR_SIZE], dir[2 * SECTOR_SIZE], sec[SECTOR_SIZE];
  char fileName[FILE_NAME_LENGTH], parents[FILE_ENTRY_TOTAL][FILE_NAME_LENGTH];
  
  interrupt(0x21, 0x0002, map, MAP_SECTOR, 0); // readSector
  interrupt(0x21, 0x0002, dir, MAP_SECTOR, 0);
  interrupt(0x21, 0x0002, dir + SECTOR_SIZE, ROOT_SECTOR+1, 0);
  interrupt(0x21, 0x0002, sec, SECTORS_SECTOR, 0);

  // adjust parent index ke index tujuan
  j = parsePath(path, parents, fileName);
  if (j != 0) {
    clear(path, strlen(path));
    strncpy(path, parents[0], strlen(parents[0]));
    strncat(path, "/", 14);
    for (i = 1; i < j; ++i) {
      strncat(path, parents[i], strlen(parents[i]));
      strncat(path, "/", 2);
    }
    parentIndex = getFileIndex(path, parentIndex, dir) & 0xFF;
  }
  // akibat dari path yang diberikan tidak valid
  if (parentIndex < 0) {
    *result = -2;
    return;
  }

  // ngecek filenya ada atau ga
  i = 0;
  while (i < 2 * SECTOR_SIZE && !exists) {
    parentExists = !parentExists ? *(dir + i) == parentIndex : parentExists;
    strncpy(s, (dir + i + 1), 1);
    exists = *(dir + i) == parentIndex &&
                    strncmp(dir + i + 2, fileName, FILE_NAME_LENGTH) == 0;
    i += 0x10;
  }
  if (!exists) {
    *result = -1;
    return;
  }
  if (!parentExists) {
    *result = -2;
    return;
  }

  i = 0;
  while (i < SECTOR_ENTRY_LENGTH && (sec + s * SECTOR_ENTRY_LENGTH + i) != 0) {
    strncpy(sectorsUsed + i, sec + s * SECTOR_ENTRY_LENGTH + i, 1);
    strncpy(sec + s * SECTOR_ENTRY_LENGTH + i, 0, 1);
    i++;
  }
  
  i = 0;
  while (sectorsUsed + i != 0) {
    strncpy(map + (*sectorsUsed + i), 0, 1);
    clearSector(*sectorsUsed + i);
    i++;
  }

  *result = 0;

  interrupt(0x21, 0x0003, map, MAP_SECTOR, 0); // writeSector
  interrupt(0x21, 0x0003, dir, ROOT_SECTOR, 0);
  interrupt(0x21, 0x0003, dir + SECTOR_SIZE, ROOT_SECTOR+1, 0);
  interrupt(0x21, 0x0003, sec, SECTORS_SECTOR, 0);
}

int getFileIndex(char *path, char parentIndex, char *dir) {
  char *entry;
  char tmpP[FILE_ENTRY_TOTAL][FILE_NAME_LENGTH], fname[FILE_NAME_LENGTH];
  char parents[FILE_ENTRY_TOTAL][FILE_NAME_LENGTH];
  bool found;
  int tmp = SECTOR_SIZE * 2;
  int i, j, jmlParents;

  // append fname di array tmpP
  jmlParents = parsePath(path, tmpP, fname);
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
    if (strncmp(parents[j], "..", FILE_NAME_LENGTH) == 0) {
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
                    ? strncmp(entry + 2, parents[j], FILE_NAME_LENGTH) == 0
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

int parsePath(char *path, char *parents, char *fname) {
  int i, j;

  path = path + (1 * (*path == '/'));
  j = strlen(path) - 1;
  *(path + j) = *(path + j) * (*(path + j) != '/');

  i = 0, j = 0;
  while (*path != '\0') {
    switch (*path) {
      case '/':
        *(parents + j + i) = 0;
        j += FILE_NAME_LENGTH * (i != 0);
        i = 0;
        break;
      default:
        *(parents + j + i) = *path;
        i++;
    }
    path++;
  }

  *(parents + j + i) = 0;
  strncpy(fname, parents + j, FILE_NAME_LENGTH);

  return div(j, FILE_NAME_LENGTH);
}

void setParameter(char parentIndex, char *cwdName, char *argv, int* success) {
  char buffer[SECTOR_SIZE], pi[6];
  int i, sectors;

  clear(pi, 6);
  clear(buffer, SECTOR_SIZE);
  sectors = getSectorsNeeded(argv);
  int2str(&pi, parentIndex);

  strncpy(buffer, pi, 6);
  strncpy(buffer+6, cwdName, FILE_NAME_LENGTH);
  // printString(cwdName); //x
  // printString(" <<< cwdName\r\n"); //x

  for (i = 0; i < MAXIMUM_ARGC && *(argv + i * MAXIMUM_CMD_LEN) != 0; i++) {
    strncpy((buffer + (i+1) * MAXIMUM_CMD_LEN),
            (argv + i * MAXIMUM_CMD_LEN), MAXIMUM_ARGC);
  }

  interrupt(0x21, 0x0005, buffer, "temp", &sectors); // writeFile
  // printNumber(sectors); //x
  // printString(" <<< sectors\r\n"); //x

  if (sectors > 0) *success = 1;
  else *success = 0;
}

void getParameter(char *parentIndex, char *cwdName, char *argv, int *success) {
  char buffer[SECTOR_SIZE];
  int i, result;

  interrupt(0x21, 0x0004, buffer, "temp", &result); // readFile

  *success = 0;
  if (result == -1) {
    return;
  } else {
    *success = 1;
    *parentIndex = dec2hex(str2int(buffer));
    strncpy(cwdName, buffer+6, FILE_NAME_LENGTH);

    for (i = 0; i < MAXIMUM_ARGC + 1; i++) {
      strncpy((argv + i * MAXIMUM_CMD_LEN),
              (buffer + (i+1) * MAXIMUM_CMD_LEN), MAXIMUM_ARGC);
    }
  
    return;
  }
}

int getSectorsNeeded(char *argv) {
  int i = 0, a, b;
  while (i < MAXIMUM_ARGC) {
    if (strncpy((argv + i * MAXIMUM_CMD_LEN), 0, 1) == 0) i++;
    else break;
  }
  i += 2;
  i *= MAXIMUM_CMD_LEN;
  a = div(i, SECTOR_SIZE);  // ceiling
  b = mod(i, SECTOR_SIZE);
  return b > 0 ? a + 1 : a;
}

void exec(char *path, int segment, int *success, int parentIndex) {
  interrupt(0x21, (parentIndex << 8) + 0x06, path, segment, success);
}
