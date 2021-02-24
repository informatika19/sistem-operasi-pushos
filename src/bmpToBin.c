#include <stdio.h>

inline unsigned char* ReadBmpFile(//<======CREATES A NEW IMAGE FROM A BMP FILE
  const char* filename){//<-------------------------THE NAME OF THE BMP FILE
  FILE* f=fopen(filename,"rb");//..............file must be opened as a binary
  if(!f)printf("\nCan't open \"%s\".\n",filename),exit(1);//.is the file open?
  unsigned char H[6];/*<-*/fread(H,1,6,f),rewind(f);//read beginning of header
  unsigned int N=H[2]+(H[3]<<8)+(H[4]<<16)+(H[5]<<24);//.........get file size
  
  unsigned char* I= (unsigned char)malloc(sizeof(unsigned char) * N);
  /*<-*/fread(I,1,N,f),fclose(f);
  return I;

  // unsigned char[N] 
}
// void readString(char * string) {
// }