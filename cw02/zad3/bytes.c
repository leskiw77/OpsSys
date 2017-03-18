#include "bytes.h"

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