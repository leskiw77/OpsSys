#include "printerStat.h"
#include "printerNftw.h"

int main (int argc, char ** argv) {
    if(argc != 4) {
        printf("Bledne argumenty programu");
        return 1;
    }

    // otwieramy katalog :
    DIR * directory = NULL;
    char * fullPath = argv[1];
    directory = opendir(fullPath);
    if(directory==NULL) {
        printf("Blad otwierania katalogu");
        return 1;
    }
    int size = -1;
    size = atoi(argv[2]);
    if(size == -1) {
        printf("Blednie podany rozmiar");
        return 1;
    }

    if(strcmp(argv[3],"stat")==0){
        printGreaterStat(directory,fullPath,size);
    } else if(strcmp(argv[3],"nftw")==0){
        printGreaterNftw(fullPath,100);
    } else {
        printf("Bledny tryb uruchomienia programu");
        return 1;
    }
    closedir (directory);
    return 0;
}
