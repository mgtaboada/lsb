#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>




void help(char* arg){
  printf("Usage: %s -c input_file container_file output_file:\n\tHides input_file into container_file and stores the result in output file\n",arg);
  printf("\t%s -o container_file:\n\tPrints to standard output the message hidden in container_file\n",arg);
}
int getBit(int n, char number){
  return (number >> n) & 1;
}


unsigned char setBit(int n,int value,char number){
  if(n < 8 && n >=0){
    char r;
    if(value == 1)
      r = number | (1<<n);
    else if (value == 0){
      r = number & ~(1<<n);
    }
    return r;
  }
  return (char)0;


}

void writeByte(char ci,FILE* fi, FILE* fo){
      int i;
      for(i=0;i<(sizeof(char)*8);i++){ // para todos los bits del caracter
        unsigned char co;
        unsigned char newCo;
        int val;
        val = getBit(i,ci); //bit i-esimo del caracter del fichero que se quiere esconder
        co = fgetc(fi);     //byte del fichero contenedor que se quiere modificar
        newCo = setBit(0,val,co);// byte modificado del fichero contenedor
        fputc(newCo,fo); // introducir el byte modificado en el fichero de salida;
      }
}

/**
 * Escribir un código de 7 AA y un AB para denotar fin del fichero escondido
 */
void writeEnd(FILE*fi,FILE* target){
  int i;
  for(i=0;i<7;i+=1){
    writeByte((char)0xAA,fi,target);
  }
  writeByte((char)0xAB,fi,target);
}

/**
 * Esconder un fichero fi dentro de otro fo y guardarlo en target
 */
void code(FILE* fi,FILE*fo,FILE* target){
  unsigned char ci;
  ci = fgetc(fi);
  // introducir todo el fichero a esconder
  while(!feof(fi)){

    writeByte(ci,fo,target);
    ci = fgetc(fi);
  }
  // escibir el fin de fichero
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

char readByte(FILE* fi){

  int i;
  char fc; // caracter que se lee del archivo
  char res;
  res = 0;
  for(i=0;i<(sizeof(char)*8)&& !feof(fi);i++){
    fc = fgetc(fi);
    res = (!!(fc & 1)) == 1? // si el ultimo bit i fc es 1
           res | 1<<i:    // entonces poner el bit que toque de res a 1
           res; // si no, ya está a cero
  }
  if (feof(fi)){
    fprintf(stderr,"Se ha llegado al final del fichero contenedor sin encontrar nada.\n");
    return -1;
  }
  return res;
}


/**
 * Resultado :
 * 0 si no se ha llegado al final
 * 16 si se ha llegado al final
 * i | 0 < i < 8 si no se ha llegado al final pero faltan i bytes por escribir
 */
int testEnd(char byte, // lo que se está leyendo ahora
            char* fin, // ultimos caracteres que se han leído
            int* finIndex //
            ){
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
  fseek(fi,0,SEEK_SET);
  int numeroAA; // numero de bytes 0xAA que se han leído, para comprobar si se ha terminado
  int end;//como booleano para ver si se ha terminado
  unsigned char cursor;
  numeroAA = 0;
  end = 0;
  while(!feof(fi) && !end){

    cursor = readByte(fi);

    if (cursor ==(unsigned char) 0xAA){
      numeroAA++;
    }

    end =(cursor == 255)|| ((cursor == (unsigned char) 0xAB) && (numeroAA >= 7));

    if(cursor != (unsigned char) 0xAA && !end) {
      for(;numeroAA>0;numeroAA--){
        fprintf(stdout,"%c",0xAA);
      }
      fprintf(stdout,"%c",cursor);
    }


  }
  if(!end){
    fprintf(stderr,"Se ha llegado al final del fichero contenedor sin encontrar nada.\n");
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
    if(argc > 2){
      fi = fopen(argv[2],"r");
    }
    else (fi = stdin);
    if(fi){
      decode(fi);
      fclose(fi);
    }



  }
  return 0;
}
