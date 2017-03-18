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
    printf("1) Ustawienie rygla na wybrany znak pliku\n");
    printf("2) Ustawienie rygla na wybrany znak pliku\n");
    printf("3) Wyświetlenie listy zaryglowanych znaków pliku,\n");
    // ( +nr PID procesu będącego właścicielem rygla oraz jego typie - odczyt/zapis)
    printf("4) Zwolnienie wybranego rygla\n");
    printf("5) Odczyt wybranego znaku pliku\n");
    printf("6) Zmiana wybranego znaku pliku\n");
    printf("0) Wyjscie\n : ");
}


void printFileLocks(int fd){
    int i;
    struct flock lock;
    int size;

    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    printf("Blokady : \n");
    for(i=0 ; i<size ; ++i){
        lock.l_len = 1;
        lock.l_start = i;
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;

        if(fcntl(fd, F_GETLK, &lock) == -1){
            printf("Blad w funkcji listOfLocks: \"%s\"\terrno: %d\n",strerror(errno),errno);
            return -1;
        }


        if(lock.l_type != F_UNLCK){
            printf("PID %d: ma blokade odczytu na bajcie %d\n", (int)lock.l_pid, (int)lock.l_start);
        }
        //test rygla zapisu
        lock.l_len = 1;
        lock.l_start = i;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;

        if(fcntl(fd, F_GETLK, &lock) == -1){
            printf("Blad w funkcji listOfLocks: \"%s\"\terrno: %d\n",strerror(errno),errno);
            return -1;
        }

        if(lock.l_type != F_UNLCK){
            printf("PID %d: ma blokade zapisu na bajcie %d\n", (int)lock.l_pid, (int)lock.l_start);
        }
    }
    return 0;
}

bool writeByte(int fd, int byteNumber, char byteValue){
    struct flock lck;
    lck.l_len = 1;
    lck.l_start = byteNumber;
    lck.l_type = F_WRLCK;
    lck.l_whence = SEEK_SET;

    if(fcntl(fd, F_GETLK, &lck) == -1){
        printf("Blad w funkcji writeChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return false;
    }

    if(lck.l_type != F_UNLCK){
        printf("Na ten znak zalozona jest blokada i nie da sie zapisywac\n");
        return false;
    }

    if(lseek(fd, byteNumber, SEEK_SET) == -1){
        printf("Blad w funkcji writeChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return false;
    }

    char * buff = (char*)malloc(sizeof(char));
    buff[0] = byteValue;
    if(write(fd,buff, 1) == -1){
        printf("Blad w funkcji readChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return false;
    }
    free(buff);
    return true;
}

char readChar(int fileDesc, int offset){
    struct flock lock;
    char ch[2];

    lock.l_len = 1;
    lock.l_start = offset;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;

    if(fcntl(fileDesc, F_GETLK, &lock) == -1){
        printf("Blad w funkcji readChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return -1;
    }

    if(lock.l_type == F_WRLCK){
        printf("Na ten znak zalozona jest blokada i nie da sie go odczytac\n");
        return -1;
    }

    if(lseek(fileDesc, offset, SEEK_SET) == -1){
        printf("Blad w funkcji readChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return -1;
    }

    if((read(fileDesc, &ch, 1)) == -1){
        printf("Blad w funkcji readChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return -1;
    }
    //printf("%c\n", znak);
    return ch[0];
}


int listOfLocks(int fileDesc){
    int i;
    struct flock lock;
    int size;

    size = lseek(fileDesc, 0, SEEK_END);
    lseek(fileDesc, 0, SEEK_SET);

    for(i=0 ; i<size ; ++i){
        //test rygla odczytu
        lock.l_len = 1;
        lock.l_start = i;
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;

        if(fcntl(fileDesc, F_GETLK, &lock) == -1){
            printf("Blad w funkcji listOfLocks: \"%s\"\terrno: %d\n",strerror(errno),errno);
            return -1;
        }

        if(lock.l_type != F_UNLCK){
            printf("PID %d: ma blokade odczytu na bajcie %d\n", (int)lock.l_pid, (int)lock.l_start);
        }
        //test rygla zapisu
        lock.l_len = 1;
        lock.l_start = i;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;

        if(fcntl(fileDesc, F_GETLK, &lock) == -1){
            printf("Blad w funkcji listOfLocks: \"%s\"\terrno: %d\n",strerror(errno),errno);
            return -1;
        }

        if(lock.l_type != F_UNLCK){
            printf("PID %d: ma blokade zapisu na bajcie %d\n", (int)lock.l_pid, (int)lock.l_start);
        }
    }

    return 0;
}

int unlock(int fileDesc, int offset){
    if((lock_reg(fileDesc, F_SETLK, F_UNLCK, offset, SEEK_SET, 1)) == -1){
        if(errno == EACCES || errno == EAGAIN){
            printf("System zdjal juz blokade z tego bajtu\n");
        }else{
            printf("Blad w funkcji unlock: \"%s\"\terrno: %d\n", strerror(errno), errno);
            return -1;
        }
    }
    return 0;
}


int writeLock(int fileDesc, int offset){
    if((lock_reg(fileDesc, F_SETLK, F_RDLCK, offset, SEEK_SET, 1)) == -1){
        if(errno == EACCES || errno == EAGAIN){
            printf("System przekazal juz blokade odczytu na dany znak pliku\n");
        }else{
            printf("Blad w funkcji writeLock: \"%s\"\terrno: %d\n", strerror(errno), errno);
            return -1;
        }
    }
    return 0;
}

int readLock(int fileDesc, int offset){
    if((lock_reg(fileDesc, F_SETLK, F_WRLCK, offset, SEEK_SET, 1)) == -1){
        if(errno == EACCES || errno == EAGAIN){
            printf("System przekazal juz blokade zapisu na dany znak pliku\n");
        }else{
            printf("Blad w funkcji readLock: \"%s\"\terrno: %d\n", strerror(errno), errno);
            return -1;
        }
    }
    return 0;
}

int lock_reg(int fileDesc, int cmd, int type, off_t offset, int whence, off_t len){
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    return fcntl(fileDesc, cmd, &lock);
}


int main(int argc, char * argv[]) {
    if(argc!=2) {
        printf("Bledna liczba argumentow");
        return 1;
    }
    int file;
    if((file = open(argv[1], O_RDWR)) == -1){
        printf("Blad odczytu pliku");
        return 1;
    }
    while(1) {
        printMenu();
        char command = getchar();
        switch (command){
            case '0': {
                return 0;
            }
            case '1': {
                printf("Podaj nr bajtu : ");
                //int byteNumber = 0;
                //scanf("%i",&byteNumber);
                //setLock(file,byteNumber);
            }
                break;
            case '2': {

            }
                break;
            case '3': {
                printFileLocks(file);
            }
                break;
            case '4': {

            }
                break;
            case '5':{

            }
                break;
            case '6':{
                printf("Podaj nr bajtu i wartosc: ");
                int byteNumber = 0;
                char byteValue = '0';
                scanf("%i %c",&byteNumber,&byteValue);
                if(writeByte(file,byteNumber,byteValue)) {
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