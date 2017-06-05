#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include "helpers.h"

char *concat(const char *one, const char *two) {
    char *c = malloc(sizeof(char) * (strlen(one) + strlen(two) + 1));
    strcpy(c, one);
    strcat(c, two);
    return c;
}

char *concat3(const char *s1, const char *s2, const char *s3) {
    char *c = malloc(sizeof(char) * (strlen(s1) + strlen(s2) + strlen(s3) + 1));
    strcpy(c, s1);
    strcat(c, s2);
    strcat(c, s3);
    return c;
}

/*
  Funkcja popFragment wyjmuje fragment do chara delim z *origStr, zwracajac wskazanie na ten fragment
  funkcja zmienia wskaznik podany jako argument, tak aby wskazywal na kolejna czesc lub NULL
  funkcja nie alokuje nowej pamieci (poza przypadkiem, gdy delim jest na pocz stringa, wtedy zwraca nowy pusty string)
  popFragment nie przesuwa slow w pamieci, znajduja sie w tym samym obszarze - warto przed forem skopiowac wskaznik, jesli
  chce sie potem zwolnic caly ten obszar
  gdy nic nie zostaje z *origStr, *origStr ustawia sie na NULL
  wywolane na NULL zwraca NULL
*/
char *popFragment(char **origStr, char delim) {
    if (*origStr == NULL) return NULL; // zwraca NULL jesli nie ma juz z czego popowac

    char *i = *origStr;
    while (*i != delim) {
        i++;
        if (*i == 0) {
            char *ret = *origStr;
            *origStr = NULL;
            return ret;
        }
    }

    if (i == *origStr) {
        *origStr = i + 1;
        char *ret = malloc(sizeof(char));
        *ret = 0;
        return ret;
    }

    *i = 0;
    char *ret = *origStr;
    if (i[1] != 0) {
        *origStr = i + 1;
    } else {
        *origStr = NULL;
    }
    return ret;
}

char *trimWhite(char *origStr) {
    char *buffer = malloc(sizeof(char) * 200);
    char *i = origStr;
    while (*i == ' ') i++;
    int j = 0;
    while (*i != 0) {
        while ((*i != ' ') && (*i != 0)) {
            buffer[j++] = *i;
            i++;
        }
        if (*i == ' ') {
            buffer[j++] = ' ';
            while (*i == ' ') i++;
        }
    }
    buffer[j++] = 0;
    buffer = realloc(buffer, j); // bezcenny ram
    return buffer;
}

void throwError(const char *err) {
    printf("Error! %s Errno: %d, %s\n", err, errno, strerror(errno));
    exit(3);
}

char *convertTime(const time_t *mtime) {
    char *buff = malloc(sizeof(char) * 30);
    struct tm *timeinfo;
    timeinfo = localtime(mtime);
    strftime(buff, 20, "%b %d %H:%M", timeinfo);
    return buff;
}

int getQID(char *path, int ID) {
    int key = ftok(path, ID);
    if (key == -1) throwError ("Generation of key failed!");

    int QID = msgget(key, 0);
    if (QID == -1) throwError ("Opening queue failed!");

    return QID;
}

long getMicroTime() {
    struct timespec marker;
    if (clock_gettime(CLOCK_MONOTONIC, &marker) == -1) throwError ("Getting time failed!");
    return marker.tv_nsec / 1000;
}

void slower(int n) {
    int *tab = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - 1; j++) {
            if (tab[j] > tab[j + 1]) swap(&tab[j], &tab[j + 1]);
        }
    }
    free(tab);
}

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int *getRandomTable(int size) {
    int *tab = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) {
        tab[i] = rand() * rand();
    }
    return tab;
}
