#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <fcntl.h>
#include <ctype.h>


int mapDoubleToInt(int maxInt, double minDouble, double maxDouble, double val) {
    val -= minDouble;
    return (int) ((val * (double) maxInt) / (maxDouble - minDouble));
}

void valuesFromString(char *str, double *x, double *y, int *v) {
    char *pch;
    pch = strtok(str, " ");
    sscanf(pch, "%lf", x);
    pch = strtok(NULL, " ");
    sscanf(pch, "%lf", y);
    pch = strtok(NULL, " ");
    sscanf(pch, "%d", v);
}


void drawGnuplot(int **T, int R) {
    // otworzenie gnuplota :
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    FILE *temp = fopen("data", "w");

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < R; ++j) {
            fprintf(temp, "%d ", T[i][j]);
        }
        fprintf(temp, "\n");
    }

    char tmpSet[10];
    sprintf(tmpSet, "%d", R - 1);

    char *setRange = (char *) malloc(sizeof(char) * 100);
    setRange[0] = '\0';
    strcat(setRange, "set xrange [0:");
    strcat(setRange, tmpSet);
    strcat(setRange, "] \n");

    // wpisanie komend do gnuplota :
    fputs("set view map \n", gnuplotPipe);
    fputs(setRange, gnuplotPipe);
    setRange[4] = 'y';
    fputs(setRange, gnuplotPipe);

    fputs("plot 'data' matrix with image \n", gnuplotPipe);

    free(setRange);

    fclose(temp);
    fclose(gnuplotPipe);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong amount of arguments\n");
        return -1;
    }
    char *pathname = argv[1];
    int R = atoi(argv[2]);

    int **T;
    T = (int **) malloc(R * sizeof(int *));
    for (int i = 0; i < R; ++i) {
        T[i] = (int *) malloc(R * sizeof(int));
    }

    unlink(pathname);

    if (mkfifo(pathname, 0777) == -1) {
        printf("Cannot create FIFO special file\n");
        exit(1);
    } else {
        printf("Created\n");
    }

    int fileDesc = open(pathname, O_RDONLY | O_NONBLOCK);
    if (fileDesc < 0) { exit(1); }

    char buf[50];

    int nread;
    int flag = 0;
    int l;
    while (1) {
        // odczytujemy dane z potoku :
        nread = read(fileDesc, buf, sizeof(buf));
        if (nread > 0) {
            l++;
            if (l % 1000 == 0) {
                printf("working %d\n", l);
            }
            // ustaw wartosci do tablicy :
            double x, y;
            int v;
            valuesFromString(buf, &x, &y, &v);
            T[mapDoubleToInt(R, -2, 1, x)][mapDoubleToInt(R, -1, 1, y)] = v;
            flag = 1;
        } else {
            if (flag == 0) {
                // zanim slave'y sie uruchomia :
                printf("wait\n");
                sleep(1);
                continue;
            } else if (flag == 1) {
                //czytalismy juz, dajemy czas slaveom
                printf("wait for data\n");
                sleep(1);
                flag = 2;
            } else {
                // wystarczajaco dluga przerwa w odczycie
                // wiec odczyt zakonczony
                break;
            }
        }
    }

    // wykonaj w gnuplocie :
    printf("Received %d data blocks ", l);
    drawGnuplot(T, R);

    for (int i = 0; i < R; ++i) { free(T[i]); }
    free(T);
    unlink(pathname);
    return 0;
}

