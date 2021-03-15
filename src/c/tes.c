#include <stdio.h>

int strLength(char* string){
  int i = 0;
  while(string[i]!='\0'){
    printf("%c",string[i]);
    i++;
  }
  return i;
}

int strCompare(char* string1,char* string2){
  int same = 1;
  int i=0;
  if(strLength(string1)==strLength(string2)){
    while (same==1 && i<strLength(string1)){
      if(string1[i]!=string2[i]){
        same = 0;
      }
      else{
        i++;
      }
    }
    return same==1;
  }
  else{
    return 0;
  }
}

int strcmp(char *s1, char *s2) {
  for (; *s1 && *s1 == *s2; s1++, s2++);
  return *(unsigned char*)s1 - *(unsigned char*)s2;
}


int main(){
  // get file name from path
  char* path="/home/tes/heraa.txt";
  char fileName[14];
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
  int x = strcmp(fileName2,"heraa.txt");
  printf("%d",x);
  // end get file name from path
    // int x = strCompare("hera","hera");
    // printf("%d",x);
}
