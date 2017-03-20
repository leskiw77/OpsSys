#include "printerStat.h"

void printGreaterStat(char *dirPath, int maxSize) {
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        printf("Blad otwierania katalogu %s\n", dirPath);
        return;
    }
    struct dirent *file;
    struct stat st;
    while ((file = readdir(dir)) != NULL) {

        // tylko pliki regularne :
        if (file->d_type == 8) {
            // pelna nazwa pliku :
            char name[256];
            strcpy(name, dirPath);
            strcat(name, file->d_name);

            // pobierz informacje o pliku :
            stat(name, &st);

            // rozmiar :
            long size = st.st_size;

            if (size <= maxSize) {
                // czas ostatniej modyfikacji :
                char time[20];
                time_t modTime = st.st_mtime;
                strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&modTime));

                // prawa dostepu :
                char accMode[10];
                for (int i = 0; i < 9; i++) accMode[i] = '-';
                if (st.st_mode & S_IRUSR) accMode[0] = 'r';
                if (st.st_mode & S_IWUSR) accMode[1] = 'w';
                if (st.st_mode & S_IXUSR) accMode[2] = 'x';
                if (st.st_mode & S_IRGRP) accMode[3] = 'r';
                if (st.st_mode & S_IWGRP) accMode[4] = 'w';
                if (st.st_mode & S_IXGRP) accMode[5] = 'x';
                if (st.st_mode & S_IROTH) accMode[6] = 'r';
                if (st.st_mode & S_IWOTH) accMode[7] = 'w';
                if (st.st_mode & S_IXOTH) accMode[8] = 'x';
                accMode[9] = '\0';

                // wypisz na ekran :
                printf("%s   %li   %s   %s\n", name, size, accMode, time);
            }
        } else if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0 && file->d_type == 4) {
            // jesli katalog to :
            char innerDirPath[256];
            strcpy(innerDirPath, dirPath);
            strcat(innerDirPath, file->d_name);
            strcat(innerDirPath, "/");
            printGreaterStat(innerDirPath,maxSize);
        }
    }
    closedir(dir);
}
