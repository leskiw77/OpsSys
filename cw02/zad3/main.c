#include "locks.h"
#include "bytes.h"

void printMenu(void) {
    printf("\n%s", "MENU : \n");
    printf("1) Ustawienie rygla na odczyt wybranego znaku pliku\n");
    printf("2) Ustawienie rygla na zapis wybranego znaku pliku\n");
    printf("3) Wyświetlenie listy zaryglowanych znaków pliku,\n");
    printf("4) Zwolnienie wybranego rygla\n");
    printf("5) Odczyt wybranego znaku pliku\n");
    printf("6) Zmiana wybranego znaku pliku\n");
    printf("0) Wyjscie\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Bledna liczba argumentow");
        return 1;
    }
    int file;
    if ((file = open(argv[1], O_RDWR)) == -1) {
        printf("Blad odczytu pliku");
        return 1;
    }

    char * command;
    printMenu();

    while (1) {
        printf(" command > ");
        scanf("%s",command);
        if(strcmp(command,"0")==0) {
            return 0;
        } else if(strcmp(command,"1")==0) {
            printf("Podaj nr bajtu : ");
            int byteNumber = 0;
            scanf("%i", &byteNumber);
            if (setLock(file, byteNumber, 'r')) {
                printf("Blokada odczytu ustawiona\n");
            }
        } else if(strcmp(command,"2")==0) {
            printf("Podaj nr bajtu : ");
            int byteNumber = 0;
            scanf("%i", &byteNumber);
            if (setLock(file, byteNumber, 'w')) {
                printf("Blokada zapisu ustawiona\n");
            }
        } else if(strcmp(command,"3")==0) {
            if (printLocks(file)) {
                printf("OK\n");
            }
        } else if(strcmp(command,"4")==0) {
            printf("Podaj nr bajtu : ");
            int byteNumber = 0;
            scanf("%i", &byteNumber);
            if (resetLock(file, byteNumber)) {
                printf("Blokada zdjeta\n");
            }
        } else if(strcmp(command,"5")==0) {
            printf("Podaj nr bajtu : ");
            int byteNumber = 0;
            char byteValue;
            scanf("%i", &byteNumber);
            if (readByte(file, byteNumber, &byteValue)) {
                printf("Odczytana wartosc : %c\n", byteValue);
            }
        } else if(strcmp(command,"6")==0) {
            printf("Podaj nr bajtu i wartosc : ");
            int byteNumber = 0;
            char byteValue = '0';
            scanf("%i %c", &byteNumber, &byteValue);
            if (writeByte(file, byteNumber, byteValue)) {
                printf("OK\n");
            }
        } else if(strcmp(command,"h")==0) {
            printMenu();
            printf("\n");
        }
    }
    return 0;
}