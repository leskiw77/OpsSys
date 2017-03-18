#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

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

bool setLock(int fd, int byteNumber, char type) {
    if (type != 'w' && type != 'r') {
        return false;
    }
    struct flock lck;
    if (type == 'w') {
        lck.l_type = F_WRLCK;
    } else {
        lck.l_type = F_RDLCK;
    };
    lck.l_start = byteNumber;
    lck.l_whence = SEEK_SET;
    lck.l_len = 1;
    if (fcntl(fd, F_SETLK, &lck) == -1) {
        if (errno != EACCES && errno != EAGAIN) {
            printf("Blad 'fcntl' : %s errno: %d", strerror(errno), errno);
            return false;
        }
    }
    return true;
}

bool resetLock(int fd, int byteNumber) {
    struct flock lck;
    lck.l_type = F_UNLCK;
    lck.l_start = byteNumber;
    lck.l_whence = SEEK_SET;
    lck.l_len = 1;
    if (fcntl(fd, F_SETLK, &lck) == -1) {
        if (errno != EACCES && errno != EAGAIN) {
            printf("Blad 'fcntl' : %s \nerrno: %d", strerror(errno), errno);
            return false;
        }
    }
    return true;
}

bool printLocks(int fd) {
    int i;
    struct flock lck;
    int size;

    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    for (i = 0; i < size; i++) {
        // blokady odczytu :
        lck.l_start = i;
        lck.l_len = 1;
        lck.l_type = F_RDLCK;
        lck.l_whence = SEEK_SET;

        if (fcntl(fd, F_GETLK, &lck) == -1) {
            printf("Blad 'fcntl' : %s \nerrno: %d", strerror(errno), errno);
            return false;
        }

        if (lck.l_type != F_UNLCK) {
            printf("Blokada odczytu : Bajt nr : %d, PID %d \n", (int) lck.l_start, (int) lck.l_pid);
        }

        // blokady zapisu :
        lck.l_start = i;
        lck.l_len = 1;
        lck.l_type = F_WRLCK;
        lck.l_whence = SEEK_SET;

        if (fcntl(fd, F_GETLK, &lck) == -1) {
            printf("Blad 'fcntl' : %s \nerrno: %d", strerror(errno), errno);
            return false;
        }

        if (lck.l_type != F_UNLCK) {
            printf("Blokada zapisu : Bajt nr : %d, PID %d \n", (int) lck.l_start, (int) lck.l_pid);
        }
    }
    return true;
}


bool writeByte(int fd, int byteNumber, char byteValue) {
    struct flock lck;
    lck.l_len = 1;
    lck.l_start = byteNumber;
    lck.l_type = F_WRLCK;
    lck.l_whence = SEEK_SET;

    if (fcntl(fd, F_GETLK, &lck) == -1) {
        printf("Blad 'fcntl' : %s \nerrno: %d", strerror(errno), errno);
        return false;
    }
    if (lck.l_type != F_UNLCK) {
        printf("Blokada uniemozliwia zapis");
        return false;
    }
    if (lseek(fd, byteNumber, SEEK_SET) == -1) {
        printf("Blad 'lseek' : %s \nerrno: %d", strerror(errno), errno);
        return false;
    }
    char *buff = (char *) malloc(sizeof(char));
    buff[0] = byteValue;
    if (write(fd, buff, 1) == -1) {
        printf("Blad 'write' : %s \nerrno: %d", strerror(errno), errno);
        return false;
    }
    free(buff);
    return true;
}

bool readByte(int fd, int byteNumber, char *value) {
    struct flock lck;
    lck.l_len = 1;
    lck.l_start = byteNumber;
    lck.l_type = F_RDLCK;
    lck.l_whence = SEEK_SET;

    if (fcntl(fd, F_GETLK, &lck) == -1) {
        printf("Blad 'fcntl' : %s \nerrno: %d", strerror(errno), errno);
        return false;
    }

    if (lck.l_type == F_WRLCK) {
        printf("Blokada uniemozliwia odczyt");
        return false;
    }

    if (lseek(fd, byteNumber, SEEK_SET) == -1) {
        printf("Blad 'lseek' : %s \nerrno: %d", strerror(errno), errno);
        return false;
    }

    char buff[2];
    if ((read(fd, &buff, 1)) == -1) {
        printf("Blad 'read' : %s \nerrno: %d", strerror(errno), errno);
        return false;
    }
    *value = buff[0];
    return true;
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
    while (1) {
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
            default: {
                printf("Nieznana komenda");
            }
        }
    }
}