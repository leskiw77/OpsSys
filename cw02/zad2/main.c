#include "printerStat.h"
#include "printerNftw.h"

int main (int argc, char ** argv) {
    if(argc != 4) {
        printf("Bledne argumenty programu");
        return 1;
    }

    char fullPath[256];
    strcpy(fullPath, argv[1]);
    if(fullPath[strlen(fullPath)-1] != '/') {
        strcat(fullPath, "/");
    }

    int size = -1;
    size = atoi(argv[2]);
    if(size == -1) {
        printf("Blednie podany rozmiar");
        return 1;
    }

    printf("\n-------------------------------\n");
    if(strcmp(argv[3],"stat")==0){
        printGreaterStat(fullPath,size);
    } else if(strcmp(argv[3],"nftw")==0){
        printGreaterNftw(fullPath,size);
    } else {
        printf("Bledny tryb uruchomienia programu");
        return 1;
    }
    return 0;
}
