#include "library.h"


void libGenerate(char * fileName, int records, int bytes){
    FILE * file = fopen(fileName, "w");
    if(file==NULL){
        printf("Cant open the file");
        exit(1);
    }
    char * toSave = (char * ) malloc(sizeof(char) * bytes);

    for(int i=0;i<records;i++){

        for (int j = 0; j < bytes; ++j) {
            toSave[j]=(char)((rand()%26)+'A');
        }
        toSave[bytes-1]='\n';

        fwrite(toSave,sizeof(char),bytes,file);
    }

    free(toSave);
    fclose(file);
}

void libSort(char * fileName, int records, int bytes){

    FILE *file=fopen(fileName,"r+"); //read and update
    if( file == NULL){
        printf("Cant open the file\n");
        exit(1);
    }

    char *sort1 = malloc(bytes * sizeof(char));
    char *sort2 = malloc(bytes * sizeof(char));

    int change_bool;

    for (int i=0;i<records;i++){

        fseek(file, 0, SEEK_SET);
        fread(sort1, sizeof(char), bytes, file);

        change_bool=0;

        for(int j=0;j<records-1;j++){

            fread(sort2, sizeof(char), bytes, file);

            if(sort1[0]>sort2[0]){
                //swap
                fseek(file, j*bytes, SEEK_SET);
                fwrite(sort2, sizeof(char), bytes, file);
                fwrite(sort1, sizeof(char), bytes, file);

                change_bool=1;
            }
            else{
                strcpy(sort1,sort2); //moze być blad jak nie bedzie konczyć sie \n
            }
        }

        if(change_bool == 0){
            break;
        }
    }

    free(sort1);
    free(sort2);

    fclose(file);
}

void swapByIndexLib(FILE *file, int x, int y, int bytes){

    char *swap1 = malloc(bytes * sizeof(char));
    char *swap2 = malloc(bytes * sizeof(char));

    fseek(file, x*bytes, SEEK_SET);
    fread(swap1, sizeof(char), bytes, file);

    fseek(file, y*bytes, SEEK_SET);
    fread(swap2, sizeof(char), bytes, file);

    fseek(file, x*bytes, SEEK_SET);
    fwrite(swap2, sizeof(char), bytes, file);

    fseek(file, y*bytes, SEEK_SET);
    fwrite(swap1, sizeof(char), bytes, file);

    free(swap1);
    free(swap2);
}


void libShuffle(char * fileName, int records, int bytes){

    FILE *file=fopen(fileName,"r+"); //read and update
    if( file == NULL){
        printf("Cant open the file\n");
        exit(1);
    }

    for (int i = 0; i < records-1; ++i) {
        int to = rand()%(records-i)+i;
        swapByIndexLib(file,i,to,bytes);
    }

}