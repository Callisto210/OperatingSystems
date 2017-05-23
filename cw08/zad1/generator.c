#include <stdlib.h>
#include <stdio.h>

#define RECORD_SIZE 1024
#define NAME "rekordy.txt"
#define AMMOUNT 1337


int main(){
  int i,id,j;
  FILE * fp;
  char rekord[RECORD_SIZE];
  
  fp =fopen(NAME,"w+");
  id=0;

  //tworzymy rekordy
  for(i=0;i<AMMOUNT;i++){
    //najpierw numer
    j=sprintf(rekord, "%4d", id);
	id++;
    rekord[j]='\0';
    //potem reszta
    for(j++;j<RECORD_SIZE-1;j++){
      rekord[j]='a'+(rand()%('z'-'a'));
    //  if(! (rand()%10))
    //    rekord[j]=' ';
    }
    rekord[j-1]='\0';
    rekord[j]='\n';
    fwrite(rekord,1,RECORD_SIZE,fp);
  }

  fclose(fp);
  return 0;
}
