#include "locks.h"

bool setLock(int fd, int byteNumber, char type) {
    if (type != 'w' && type != 'r') {
        return false;
    }
    struct flock lck;
    if (type == 'w') {
        lck.l_type = F_RDLCK;
    } else {
        lck.l_type = F_WRLCK;
    }
    lck.l_start = byteNumber;
    lck.l_whence = SEEK_SET;
    lck.l_len = 1;
    if (fcntl(fd, F_SETLK, &lck) == -1) {
        if (errno == EACCES || errno == EAGAIN) {
            printf("Ustawiono lock na podany bajt. Czy chcesz zaczekać na usuniecie? [t/n]");

            char c ;
            while ((c = getchar()) != '\n' && c != EOF) { ;}
            c = getchar();

            if (c == 't') {
                if(fcntl(fd, F_SETLKW, &lck) == -1){
                    printf("Unexpected error\n");
                }
            } else{
                return false;
            }


        } else{
            printf("Unexpected error\n");
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
        printf("trurur");
        if (errno == EACCES || errno == EAGAIN) {
            printf("Brak locka?");

        }
        if (errno != EACCES && errno != EAGAIN) { //permition denied, temporarily unavailable
            printf("Blad 'fcntl' : %s \nerrno: %d", strerror(errno), errno);
            return false;
        }
    }
    printf("chuj");

    if (lck.l_type != F_UNLCK) {
     printf("Brak locka\n");
    }
    return true;
}

bool printLocks(int fd) {
    struct flock lck;
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    for (int i = 0; i < size; i++) {
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