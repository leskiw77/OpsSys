#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "complex.h"

#define LIMIT 2

double dRand(double fMin, double fMax) {
    double f = (double) rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int iters(complex c, int k) {
    int i = 0;
    complex n;
    n.re = n.im = 0;
    while (i < k && absolute(n) <= 2) {
        n = addComplex(doubleComplex(n), c);
        i++;
    }
    return i;
}

// args : ścieżka do potoku nazwanego i liczby całkowite N oraz K.
int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Wrong amount of arguments\n");
        return -1;
    }

    char *pathname = argv[1];
    int n, k;
    n = atoi(argv[2]);
    k = atoi(argv[3]);

    // otwieramy potok :
    int fileDesc = open(pathname, O_WRONLY);
    if (fileDesc < 0) {
        printf("error: %s\n", strerror(errno));
        exit(-1);
    }

    printf("Opened: %s\n", pathname);

    // generujemy N losowych pkt z przedzialu [-2.0, 1.0] x [-1.0, 1.0] ( liczby zespolone )
    for (int i = 0; i < n; ++i) {
        double x = dRand(-2.0, 1.0);
        double y = dRand(-1.0, 1.0);
        complex c;
        c.re = x;
        c.im = y;

        // wpisujemy do potoku wartosc iters(c)
        char s[50];
        sprintf(s, "%.19lf %.19lf %d", x, y, iters(c, k));
        write(fileDesc, s, 50 * sizeof(char));
    }
    close(fileDesc);
    printf("Finish\n");
    return 0;
}