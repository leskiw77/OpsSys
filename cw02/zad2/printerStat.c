#include "printerStat.h"

void printGreaterStat(DIR * dir, char * dirPath, int minSize) {
    struct dirent * file;
    struct stat st;
    while ((file = readdir(dir)) != NULL) {
        // tylko pliki regularne :
        if (file->d_type == 8) {
            // pelna nazwa pliku :
            char name[256];
            strcpy(name,dirPath);
            strcat(name,file->d_name);

            // pobierz informacje o pliku :
            stat(name,&st);

            // rozmiar :
            long size = st.st_size;

            if( size >= minSize ) {
                // czas ostatniej modyfikacji :
                char time[20];
                time_t modTime = st.st_mtime;
                strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&modTime));

                // prawa dostepu :
                char accMode[10];
                for(int i=0;i<9;i++)  accMode[i] = '-';
                if(st.st_mode & S_IRUSR) accMode[0] = 'r';
                if(st.st_mode & S_IWUSR) accMode[1] = 'w';
                if(st.st_mode & S_IXUSR) accMode[2] = 'x';
                if(st.st_mode & S_IRGRP) accMode[3] = 'r';
                if(st.st_mode & S_IWGRP) accMode[4] = 'w';
                if(st.st_mode & S_IXGRP) accMode[5] = 'x';
                if(st.st_mode & S_IROTH) accMode[6] = 'r';
                if(st.st_mode & S_IWOTH) accMode[7] = 'w';
                if(st.st_mode & S_IXOTH) accMode[8] = 'x';
                accMode[9] = '\0';

                // wypisz na ekran :
                printf("%s   %li   %s   %s\n", name, size, accMode, time);
            }
        }
    }
}
