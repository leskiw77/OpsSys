#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <fcntl.h>
#include <ctype.h>


#define LIMIT 10000000

int mapDoubleToInt(int maxInt, double minDouble, double maxDouble, double val ){
    val -= minDouble;
    return (int)((val*(double)maxInt)/(maxDouble - minDouble));
}


void valuesFromString(char * str, double * x, double * y, int * v){

    char * pch;

    pch = strtok (str," ");
    sscanf(pch, "%lf", x);
    pch = strtok (NULL, " ");
    sscanf(pch, "%lf", y);
    pch = strtok (NULL, " ");
    sscanf(pch, "%d", v);

}


void drowGnuplot(int ** T, int R){
    FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    FILE * temp = fopen("data", "w");

    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < R; ++j) {
            fprintf(temp,"%d ",T[i][j]);
        }
        fprintf(temp,"\n");
    }

    char tmpSet[10];
    sprintf(tmpSet, "%d", R-1);

    char * setRange = (char *)malloc(sizeof(char) * 100);
    setRange[0]='\0';
    strcat(setRange,"set xrange [0:");
    strcat(setRange,tmpSet);
    strcat(setRange,"] \n");

    fputs("set view map \n", gnuplotPipe);
    fputs(setRange, gnuplotPipe);
    setRange[4]='y';
    fputs(setRange, gnuplotPipe);

    fputs("plot 'data' matrix with image \n", gnuplotPipe);

    free(setRange);

    fclose(temp);
    fclose(gnuplotPipe);
}


int main(int argc, char *argv[]) {

    //ścieżkę do potoku nazwanego i liczbę całkowitą R.
    if (argc != 3) {
        printf("Wrong amount of arguments\n");
        return -1;
    }
    char * pathname = argv[1];
    int R=atoi(argv[2]);

    int ** T;
    T=(int **)malloc(R*sizeof(int *));
    for (int i = 0; i < R; ++i) {
        T[i]= (int *)malloc(R*sizeof(int));
    }


    unlink(pathname);

    if(mkfifo(pathname, 0777) == -1){
        printf("Cannot create FIFO special file\n");
        exit(1);
    } else{
        printf("Created\n");
    }

    int filedesc = open(pathname, O_RDWR);
    if ( filedesc < 0) {

        exit(-1);
    }

    char buf[50];

    int nread;
    int lll=0;
    while(1) {
        if((nread = read(filedesc, buf, sizeof(buf))) > 0) {

            double x,y;
            int v;

            valuesFromString(buf,&x,&y,&v);

            T[mapDoubleToInt(R,-2,1,x)][mapDoubleToInt(R,-1,1,y)] = v;

        }
        lll++;
        if(lll==LIMIT) break;
    }


    drowGnuplot(T,R);



    for (int i = 0; i < R; ++i) {
        free(T[i]);
    }
    free(T);
    unlink(pathname);
    return 0;

}

