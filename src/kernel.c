void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char * string);
void readString(char * string);
void clear(char *buffer, int length);
void printLogo();
void lineLogo(char * string, int y);

#define VIDMEM 0xB000
#define OFFSET 0x8000

int main() {
  char tes[16];
  interrupt(0x10, 0x0003, 0, 0, 0);
  printLogo();
  interrupt(0x10, 0x0200, 0 , 0, 0x0C00);
  printString("Masukkan input : ");
  readString(tes);
  printString(tes);
  clear(tes,16);
  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
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
    interrupt(0x10, 0xe * 256 + *string, 0, 0, 0);
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

void printLogo() {
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`",0);
  lineLogo("           `...........`   `...`      ...`   `.....``  `..`     `..`",1);
  lineLogo("           ..............` `....     `.... `.........` ....    `....",2);
  lineLogo("           .....`````..... `....     `.... ....`  ```  ....    `....",3);
  lineLogo("           ..--`     `.... `....     ..... .....``     ....``` `....",4);
  lineLogo("           //ooo+:-...-:////. -:-..     :o/:. `../+oo/.   ..-/+oo+:-",5);
  lineLogo("           +ooo+//:-:::.`  /oo+:     /ooo/   `-/oooo+``+ooooooooo+//",6);
  lineLogo("           +ooo:  ``````.  -ooo+`   `/ooo:      .+ooo:`oooo.```-ooo+",7);
  lineLogo("           +ooo:  -------.  /ooo+:-:+ooo/``/+/:-:+ooo.`ooo+`   -ooo+",8);
  lineLogo("           /ooo-  +++++:.-. `-+ooooooo+-` `+ooooooo+- `ooo+`   .ooo/",9);
  lineLogo("           `-:.  .++++++:--.` ``.-:-.`` `` ``.-::-.` ` `--` `.  .--`",10);
}
                                                                  
void lineLogo(char * string, int y) {
  int j = 0;
  while (string[j] != '\0') {
    putInMemory(VIDMEM, OFFSET + (80 * y + j) * 2, string[j]);
    j++;
    switch (string[j]) {
    case 'o':
      putInMemory(VIDMEM, OFFSET + (80 * y + j) * 2 + 1, 0xE);
      break;
    case '.':
      putInMemory(VIDMEM, OFFSET + (80 * y + j) * 2 + 1, 0x9);
      break;
    default:
     putInMemory(VIDMEM, OFFSET + (80 * y + j) * 2 + 1, 0xF);
    }
  }
}
