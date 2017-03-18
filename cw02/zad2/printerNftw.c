#include "printerNftw.h"

/*
 *  TODO : 1) poprawić zeby wyswietlało tylko wieksze od zadanego rozmiaru
 *         2) niech wyswietla tylko pliki regularne
 *
 */
static int printSingleFile (const char *fpath, const struct stat *sb, int typeflag){

    // rozmiar pliku :
    long size = sb->st_size;

    // czas ostatniej modyfikacji :
    char time[20];
    time_t modTime = sb->st_mtime;
    strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&modTime));

    // prawa dostepu :
    char accMode[10];
    convertAccessMode(accMode,sb->st_mode);

    printf("%s   %li   %s   %s\n", fpath, size, accMode, time);
    return 0; // necessary to continue tree walking
}


void printGreaterNftw(char * dir, int size) {
    int flags = 0;
    flags |= FTW_F;
    nftw(dir, printSingleFile,20,flags);
}


void convertAccessMode(char res[10], mode_t accMode) {
    for(int i=0;i<9;i++)  res[i] = '-';
    if(accMode & S_IRUSR) res[0] = 'r';
    if(accMode & S_IWUSR) res[1] = 'w';
    if(accMode & S_IXUSR) res[2] = 'x';
    if(accMode & S_IRGRP) res[3] = 'r';
    if(accMode & S_IWGRP) res[4] = 'w';
    if(accMode & S_IXGRP) res[5] = 'x';
    if(accMode & S_IROTH) res[6] = 'r';
    if(accMode & S_IWOTH) res[7] = 'w';
    if(accMode & S_IXOTH) res[8] = 'x';
    res[9] = '\0';
}
