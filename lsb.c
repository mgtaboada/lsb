#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>




void help(char* arg){
  printf("Uso: %s -c archivoA archivoB archivoC: Codifica archivoA dentro de archivoB y guarda el resultado en archivoC\n",arg[0]);
  printf("\t%s -o archivoA: extrae un archivo codificado de archivoA y lo muestra por consola\n",arg[0]);
}
int getBit(int n, char number){
  return (number >> n) & 1;
}


unsigned char setBit(int n,int value,char number){
  if(n < 8 && n >=0){
    char r;
    if(value == 1)
      r = number | (int)pow(2.0,n);
    else if (value == 0){
      r = number & (255 - (int)pow(2.0,n));
    }
    return r;
  }
  return (char)0;


}
void writeByte(char ci,FILE* fi, FILE* fo){
      int i;
    for(i=0;i<8;i++){
      unsigned char co;
      unsigned char newCo;
      int val;
      val = getBit(i,ci);
      co = fgetc(fi);
      newCo = setBit(0,val,co);
      fputc(newCo,fo);
    }

}
void writeEnd(FILE*fi,FILE* target){
  int i;
  for(i=0;i<7;i+=2){
    writeByte(0xAA,fi,target);
  }
  writeByte(0xAB,fi,target);
}
void code(FILE* fi,FILE*fo,FILE* target){
  unsigned char ci;
  ci = fgetc(fi);

  while(!feof(fi)){

    writeByte(ci,fo,target);
    ci = fgetc(fi);
  }

  writeEnd(fo,target);

  while(!feof(fo)){
    fputc(fgetc(fo),target); // copiar el resto del archivo
  }
}
int testEnd(char byte,int* currentAARead){
  int result;
  result = 0; //False
  if(*currentAARead < 7 && byte == 0xAA ){
    *currentAARead+=1;

  }
  if(*currentAARead == 7 && byte == 0xAB){
    result = 1;//True
    *currentAARead = 0;
  }
  else{
    *currentAARead = 0;
  }
  return result;
}
void decode(FILE* fi){
  rewind(fi);
    unsigned char ci;
    unsigned char co;
  ci = fgetc(fi);
  int i = 0;
  int currentAARead = 0; //contador para comprobar si se ha terminado de leer el archivo
  co = 0;
  while(!feof(fi)){
    if(i == 8){ // si se ha terminado de leer un byte

      putchar(co);

      testEnd(co,&currentAARead);

      co = 0;
      i = 0;
    }
    else{
      int val;
      val = getBit(0,ci);

      co = setBit(i,val,co);
      i++;
      ci = fgetc(fi);
    }


  }
}

int main(int argc, char** argv){
  if(argc == 1 || argc > 5 || strcmp(argv[1],"-h") == 0){
    help(argv[0]);
    exit(0);
  }

  if(strcmp(argv[1],"-c") == 0){
    FILE* fi;
    FILE* fo;
    FILE* t;
    fi = fopen(argv[2],"rb");
    fo = fopen(argv[3],"rb");
    t = fopen(argv[4],"wb");
    if(fi && fo)
      code(fi,fo,t);
    fclose(fi);
    fclose(fo);
    fclose(t);
  }
    if(strcmp(argv[1],"-o") == 0){
    FILE* fi;
      fi = fopen(argv[2],"r");
    if(fi)
      decode(fi);
    fclose(fi);


  }
  return 0;
}
