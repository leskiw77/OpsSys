#include "system.h"

void sysGenerate(char * fileName, int records, int bytes){
    int filedesc;
    char * toSave = (char *) malloc(sizeof(char) * bytes);
    //S_IRUSR	| S_IWUSR	| S_IXUSR
    if((filedesc=open(fileName, O_WRONLY | O_TRUNC | O_CREAT,0666)) == -1){
        printf("Cant open the file");
        exit(1);
    }


    for(int i=0;i<records;i++){

        for (int j = 0; j < bytes; ++j) {
            toSave[j]=(char)((rand()%26)+'A');
        }
        toSave[bytes-1]='\n';

        write(filedesc, toSave, bytes);
    }
    close(filedesc);
}

void sysSort(char * fileName, int records, int bytes){

    int filedesc=open(fileName,O_RDWR);
    if(filedesc < 0){
        printf("Cant open the file\n");
        exit(1);
    }

    char *sort1 = malloc(bytes * sizeof(char));
    char *sort2 = malloc(bytes * sizeof(char));

    int change_bool;

    for (int i=0;i<records;i++){

        lseek(filedesc, 0, SEEK_SET);
        read(filedesc, sort1, bytes);

        change_bool=0;

        for(int j=0;j<records-1;j++){

            read(filedesc, sort2, bytes);

            if(sort1[0]>sort2[0]){
                //swap
                lseek(filedesc, j*bytes, SEEK_SET);
                write(filedesc, sort2, bytes);
                write(filedesc, sort1, bytes);

                change_bool=1;
            }
            else{
                strcpy(sort1,sort2); //moze być blad jak nie bedzie konczyć sie \n
            }
        }

        if(!change_bool){
            break;
        }
    }

    free(sort1);
    free(sort2);
}

void swapByIndexSys(int filedesc, int x, int y, int bytes){

    char *swap1 = malloc(bytes * sizeof(char));
    char *swap2 = malloc(bytes * sizeof(char));

    lseek(filedesc, x*bytes, SEEK_SET);
    read(filedesc, swap1, bytes);

    lseek(filedesc, y*bytes, SEEK_SET);
    read(filedesc, swap2, bytes);

    lseek(filedesc, x*bytes, SEEK_SET);
    write(filedesc, swap2, bytes);

    lseek(filedesc, y*bytes, SEEK_SET);
    write(filedesc, swap1, bytes);

    free(swap1);
    free(swap2);
}


void sysShuffle(char * fileName, int records, int bytes){

    int filedesc=open(fileName,O_RDWR);
    if(filedesc < 0){
        printf("Cant open the file\n");
        exit(1);
    }

    for (int i = 0; i < records-1; ++i) {
        int to = rand()%(records-i)+i;
        swapByIndexSys(filedesc,i,to,bytes);
    }

}
