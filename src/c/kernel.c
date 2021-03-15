#include "lib/kernel.h"
#include "lib/math.h"

#define VIDMEM 0xA000
#define SCREEN_WIDTH 320
#define BLUE 0x9
#define YELLOW 0xE
#define WHITE 0xF
#define SECTOR_SIZE 512

int main() {
  char test[16];
  interrupt(0x10, 0x0013, 0, 0, 0);
  printLogoGrafik();
  interrupt(0x15, 0x8600, 0, 5, 0);
  interrupt(0x10, 0x0003, 0, 0, 0x0C00);
  // printLogoASCII();
  printString("Masukkan input : ");
  readString(test);
  printString(test);
  clear(test,16);
  while(1);

}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
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
  printString(" _____ _____ _____ _____    _____ _____ ");
  printString("|  _  |  |  |   __|  |  |  |     |   __|");
  printString("|   __|  |  |__   |     |  |  |  |__   |");
  printString("|__|  |_____|_____|__|__|  |_____|_____|");
}
                                                                  
void lineLogo(char * string, int y) {
  int j = 0;
  while (string[j] != '\0') {
    // putInMemory(VIDMEM, (80 * y + j) * 2, string[j]);
    j++;
    switch (string[j]) {
    case 'o':
      putInMemory(VIDMEM, (SCREEN_WIDTH * y + j), YELLOW);
      break;
    case '.':
      putInMemory(VIDMEM, (SCREEN_WIDTH * y + j), BLUE);
      break;
    default:
     putInMemory(VIDMEM, (SCREEN_WIDTH * y + j), WHITE);
    }
  }
}

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
  interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex){


}

// int strLength(char* string){
//   int i = 0;
//   int length = 0;
//   while(string[i]!='/0'){
//     length++;
//     i++;
//   }
//   return length;
// }

// int strCompare(char* string1,char* string2){
//   int same = 1;
//   int i=0;
//   if(strLength(string1)==strLength(string2)){
//     while (same==1 && i<strLength(string1)){
//       if(string1[i]!=string2[i]){
//         same = 0;
//       }
//       else{
//         i++;
//       }
//     }
//     return same==1;
//   }
//   else{
//     return 0;
//   }
// }

int strcmp(char *s1, char *s2) {
  for (; *s1 && *s1 == *s2; s1++, s2++);
  return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, int n) {
  for (; *s1 && n && *s1 == *s2; n--, s1++, s2++);
  return (*(unsigned char*)s1 - *(unsigned char*)s2) * (n != 0);
}

int isPathValid(char *path, char parentIndex){
  if(path[0]=='.' && path[1]=='.' && parentIndex==0xFF){
    return 0;
  }
  else{
    return 1;
  }
}

char getFileIdx(char *name, char parentIndex,char *files){
  int found=0;
  int idx = 0;
  while (found==0){
    if (strncmp(name,files[idx*16+2],14) == 0 && files[idx*16]==parentIndex){
      return idx;
    }
    else{
      idx++;
    }
  }
}

int strlen(char* s) {
  int i = 0;
  while(s[i] != '\0') { i++; }
  return i;
}

int getFileIdxFromPath(char *path, char parentIndex, char *files, char *sectors){  
  if(path[0]=='.' && path[1]=='.' && parentIndex==0xFF){
    return -1;
  }
  else{ //path diawali '/' atau './'
    //studi kasus ./hera/downloads/bacot.txt
    int i=0;
    char currentIndex;
    int pathLength = strlen(path);
    while(path[i]!='\0'){
      if(path[i]=='/'){
        i++;
        int j=0;
        char *folderName;
        while((path[i]!='/' && path[i]!='\0' && i<pathLength){
          folderName[j] = path[i];
          i++;
          j++;
        }
        currentIndex = getFileIdx(folderName,parentIndex,files);
        if(files[16*currentIndex]==parentIndex){
          parentIndex = currentIndex;
        }
        else{
          return -1;
        }
      }
    }
    else{
      i++;
    }
  }
}

void readFile(char *buffer, char *path, int *result, char parentIndex){
  char files[1024];
  char sectors[512];
  char fileName[14];

  // get file name from path
  int length = 0;
  while (path[length]!='\0'){
    length++;
  }
  int i = length-1;
  int j = 0;
  while(path[i]!='/' && i>=0){
    fileName[j]=path[i];
    i--;
    j++;
  }
  //reverse
  int lengthFileName = j;
  char fileName2[lengthFileName];
  for(int i=0;i<lengthFileName;i++){
    fileName2[i]=fileName[j-1];
    j=j-1;
  }
  fileName2[lengthFileName]='\0';
  // end get file name from path

  readSector(files, 0x101);
	readSector(files + 512, 0x102);
	readSector(sectors, 0x103);

  int found=0;
  int idx = 0;
  while (found==0){
    if (strcmp(fileName2,files[idx*16+2]) == 0 && files[idx*16] == parentIndex){
      found = 1;
    }
    else{
      idx++;
    }
  }

  if(found==1){

    int noSector = 0;
    int idxSec = files[16*idx + 1];
    while(){

    }
    // // readSector(buffer + (idx * 512), sectors[idxsector]);
    // for(nosector=0;nosector<15;nosector++){
    //   int secPos = secidx*16+nosector;
		// if (sectors[secPos] == 0) {
		// 	break;
		// }
		// readSector(buffer + (nosector * 512), sectors[secPos]);

    }
  }

  else if(found==0 && idx==32){
    *result = -1;
    printf("File tidak ditemukan");
  }
}
