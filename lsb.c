#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>




void help(char* arg){
  printf("Uso: %s -c archivoA archivoB archivoC: Codifica archivoA dentro de archivoB y guarda el resultado en archivoC\n",arg);
  printf("\t%s -o archivoA: extrae un archivo codificado de archivoA y lo muestra por consola\n",arg);
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
/*int testEnd(char byte,int* currentAARead){
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
  }*/
/**
 * Resultado :
 * 0 si no se ha llegado al final
 * 16 si se ha llegado al final
 * i | 0 < i < 8 si no se ha llegado al final pero faltan i bytes por escribir
 */
int testEnd(char byte,char* fin, int* finIndex){
  int result;
  result = 0; //False
  //si se están leyendo AA seguidos
  if(*finIndex < 7 && byte == 0xAA ){
    fin[*finIndex] = byte;
    *finIndex+=1;

  }
  // si se han terminado de leer AA y se lee AB
  if(*finIndex == 7 && byte == 0xAB){
    result = 16;//True
    *finIndex = 0;
  }
  // si se han estado leyendo AA pero no continua la secuencia, eran del archivo
  if(*finIndex < 7 && byte != 0xAA){
    result = *finIndex;
    *finIndex = 0;
  }
  // si se lee un caracter distinto, se devuelve 0
  else{
    *finIndex = 0;
  }
  return result;
  }
void decode(FILE* fi){
  rewind(fi);
    unsigned char ci;
    unsigned char co;
  ci = fgetc(fi);
  int i = 0;//numero de bits leidos
  int currentAARead = 0; //contador para comprobar si se ha terminado de leer el archivo
  char fin[8];
  int finIndex = 0;
  co = 0;//caracter que se va a escribir
  int end = 0;
  while(!feof(fi) && !end){
    if(i == 8){ // si se ha terminado de leer un byte


      //comprobar si se ha terminado
      end = testEnd(co,fin,&finIndex);
      if(end == 0){
        putchar(co);
      }
      else if (end < 16){
        int j;
        for(j = 0; j < finIndex;j++){
          putchar(fin[j]);
        }
        end = 0;
      }
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
