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
    printf("0) Wyjscie\n : ");
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
    printMenu();
    char command = getchar();
    switch (command) {
        case '0': {
            return 0;
        }
        case '1': {
            printf("Podaj nr bajtu : ");
            int byteNumber = 0;
            scanf("%i", &byteNumber);
            if (setLock(file, byteNumber, 'r')) {
                printf("Blokada odczytu ustawiona\n");
            }
        }
            break;
        case '2': {
            printf("Podaj nr bajtu : ");
            int byteNumber = 0;
            scanf("%i", &byteNumber);
            if (setLock(file, byteNumber, 'w')) {
                printf("Blokada zapisu ustawiona\n");
            }
        }
            break;
        case '3': {
            if (printLocks(file)) {
                printf("OK\n");
            }
        }
            break;
        case '4': {
            printf("Podaj nr bajtu : ");
            int byteNumber = 0;
            scanf("%i", &byteNumber);
            if (resetLock(file, byteNumber)) {
                printf("Blokada zdjeta\n");
            }
        }
            break;
        case '5': {
            printf("Podaj nr bajtu : ");
            int byteNumber = 0;
            char byteValue;
            scanf("%i", &byteNumber);
            if (readByte(file, byteNumber, &byteValue)) {
                printf("Odczytana wartosc : %c\n", byteValue);
            }
        }
            break;
        case '6': {
            printf("Podaj nr bajtu i wartosc : ");
            int byteNumber = 0;
            char byteValue = '0';
            scanf("%i %c", &byteNumber, &byteValue);
            if (writeByte(file, byteNumber, byteValue)) {
                printf("OK\n");
            }
        }
            break;
        case 'h' : {
            printMenu();
            printf("\n\n");
        }
    }
    return 0;
}