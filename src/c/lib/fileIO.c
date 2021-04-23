#include "headers/fileIO.h"
#include "headers/string.h"
#include "headers/boolean.h"
#include "headers/math.h"

void readSector(char *buffer, int sector) {
  interrupt(0x21, 0x0002, buffer, sector, 0);
}

void writeSector(char *buffer, int sector) {
  interrupt(0x21, 0x0003, buffer, sector, 0);
}

/**
 * @param sector index sector di system img
 */
void clearSector(int sector) {
  char buffer[SECTOR_SIZE];
  int i;
  clear(buffer, SECTOR_SIZE);
  for (i = 0; i < SECTOR_SIZE; i++) {
    *(buffer + i) = 0;
  }
  writeSector(buffer, sector);
}

void clear(char *buffer, int length) {
  int i;
  for (i = 0; i < length; i++) {
    *(buffer + i) = 0;
  }
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
  interrupt(0x21, (parentIndex << 8) + 0x04, buffer, path, result); // readFile
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
  interrupt(0x21, (parentIndex << 8) + 0x05, buffer, path, sectors); // writeFile
}

/**
 * @param result
 * -1 file tidak ada
 * -2 path tidak valid
 */
void removeFile(char *path, int *result, char parentIndex) {
  int i, j, sectorsUsed[16]; char s;
  // char temp;
  bool exists = false, parentExists = (parentIndex == '\xFF');
  char map[SECTOR_SIZE], dir[2 * SECTOR_SIZE], sec[SECTOR_SIZE];
  char fileName[FILE_NAME_LENGTH], parents[FILE_ENTRY_TOTAL][FILE_NAME_LENGTH];
  
  readSector(map, MAP_SECTOR);
  readSector(dir, ROOT_SECTOR);
  readSector(dir+SECTOR_TOTAL, ROOT_SECTOR+1);
  readSector(sec, SECTORS_SECTOR);

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
    strncpy(&s, (dir + i + 1), 1);
    exists = *(dir + i) == parentIndex &&
                    strncmp(dir + i + 2, fileName, FILE_NAME_LENGTH) == 0;
    i += FILE_ENTRY_LENGTH;
  }
  if (!exists) {
    *result = -1;
    return;
  }
  if (!parentExists) {
    *result = -2;
    return;
  }

  // cari index sector yang di pake dari sektor sectors
  strncpy(&sectorsUsed, sec + s * SECTOR_ENTRY_LENGTH, SECTOR_ENTRY_LENGTH); 

  // balikin 0xFF ke 0x00 di map + buang isi sector yang dipake
  i = 0;
  while (*(sectorsUsed + i) != 0) {
    *(map + *(sectorsUsed + i)) = 0;
    clearSector(*(sectorsUsed + i));
    i++;
  }

  // bersihin isi sektor sector entry ke-s
  clear((sec + s * SECTOR_ENTRY_LENGTH), SECTOR_ENTRY_LENGTH);

  // bersihin entry di dir
  i = 0;
  while (i < FILE_ENTRY_TOTAL) {
    if (*(dir + i * FILE_ENTRY_LENGTH) == parentIndex &&
        *(dir + i * FILE_ENTRY_LENGTH + 1) == s &&
        strncmp((dir + i * FILE_ENTRY_LENGTH + 2), fileName, FILE_NAME_LENGTH) == 0) {
          clear((dir + i * FILE_ENTRY_LENGTH), FILE_ENTRY_LENGTH);
          break;
        }
    i++;
  }

  *result = 1;

  writeSector(map, MAP_SECTOR);
  writeSector(dir, ROOT_SECTOR);
  writeSector(dir+SECTOR_SIZE, ROOT_SECTOR+1);
  writeSector(sec, SECTORS_SECTOR);
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
  int2str(&pi, parentIndex);

  strncpy(buffer, pi, 6);
  strncpy(buffer+6, cwdName, FILE_NAME_LENGTH);

  for (i = 0; i < MAXIMUM_ARGC && *(argv + i * MAXIMUM_CMD_LEN) != 0; i++) {
    strncpy((buffer + (i+1) * MAXIMUM_CMD_LEN),
            (argv + i * MAXIMUM_CMD_LEN), MAXIMUM_CMD_LEN);
  }

  sectors = getSectorsNeeded(argv);
  interrupt(0x21, 0x0005, buffer, "temp", &sectors); // writeFile

  if (sectors > 0) *success = 1;
  else *success = 0;
}

void getParameter(char *parentIndex, char *cwdName, char *argv, int *success) {
  char buffer[SECTOR_SIZE];
  int i, result, cwdIdx;

  readFile(buffer, "temp", &result, 0x00);

  *success = 0;
  if (result == -1) {
    return;
  } else {
    strncpy(cwdName, buffer+6, FILE_NAME_LENGTH);

    for (i = 0; i < MAXIMUM_ARGC + 1; i++) {
      strncpy((argv + i * MAXIMUM_CMD_LEN),
              (buffer + (i+1) * MAXIMUM_CMD_LEN), MAXIMUM_ARGC);
    }

    *parentIndex = str2int(buffer) & 0xFF;
    *success = 1;
  }
}

int getSectorsNeeded(char *argv) {
  // maybe broken becoz ada sampah di temp
  int i = 0, a, b;

  while (i < MAXIMUM_ARGC) {
    if (strncmp((argv + i * MAXIMUM_CMD_LEN), 0, 1) != 0) i++;
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
