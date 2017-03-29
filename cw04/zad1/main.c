#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

bool forward = true;

void changeDirection() {
    forward = !forward;
}

// CTRL + C
void reactSIGINT(int signal) {
    printf("\n\nReceived SIGINT\n\n");
    exit(1);
}

// CTRL + Z
void reactSIGTSTP(int signal) {
    changeDirection();
    printf("\n\nReceived SIGTSTP\n\n");
}

int main(int argc, char **argv) {
    char letter = 'A';
    forward = true;
    while(true) {
        printf("%c \n",letter);
        if(letter=='Z' && forward)       {  printf("\n"); letter = 'A'; }
        else if(letter=='A' && !forward) {  printf("\n"); letter = 'Z'; }
        else if(forward)                 {  letter++; }
        else if(!forward)                {  letter--; }

        if(signal(SIGINT,reactSIGINT) == SIG_ERR) {
            printf("\n\nError while receiving signal SIGINT");
            exit(1);
        }
        if(signal(SIGTSTP,reactSIGTSTP) == SIG_ERR) {
            printf("\n\nError while receiving signal SIGSTP");
            exit(1);
        }
        usleep(250000);
    }
}