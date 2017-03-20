#include "printerNftw.h"

static long maxSize = 0;

static int printSingleFile (const char *fpath, const struct stat *sb, int type){
    // rozmiar pliku
    long size = sb->st_size;

    if(size > maxSize) {
        return 0;
    }

    // czas ostatniej modyfikacji :
    char time[20];
    time_t modTime = sb->st_mtime;
    strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&modTime));

    // prawa dostepu :
    char accMode[10];
    mode_t fMode = sb->st_mode;
    for(int i=0;i<9;i++)  accMode[i] = '-';
    if(fMode & S_IRUSR) accMode[0] = 'r';
    if(fMode & S_IWUSR) accMode[1] = 'w';
    if(fMode & S_IXUSR) accMode[2] = 'x';
    if(fMode & S_IRGRP) accMode[3] = 'r';
    if(fMode & S_IWGRP) accMode[4] = 'w';
    if(fMode & S_IXGRP) accMode[5] = 'x';
    if(fMode & S_IROTH) accMode[6] = 'r';
    if(fMode & S_IWOTH) accMode[7] = 'w';
    if(fMode & S_IXOTH) accMode[8] = 'x';
    accMode[9] = '\0';
    
    printf("%s   %li   %s   %s\n", fpath, size, accMode, time);
    return 0;
}

void printGreaterNftw(char * dir, int size) {
    maxSize = size;
    nftw(dir, printSingleFile,FTW_F,100);
}

