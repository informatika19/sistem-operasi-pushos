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

