#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


static bool forward = true;
void changeDirection() { forward = !forward;  }


// CTRL + C
void reactSIGINT(int signal) {
    printf("\n\nReceived SIGINT -> end of the programme\n\n");
    exit(0);
}

// CTRL + Z
void reactSIGTSTP(int signal) {
    changeDirection();
    printf("\n\nReceived SIGTSTP -> changing direction\n\n");
}


int main(int argc, char **argv) {
    char letter = 'A';
    forward = true;

    // ustawienie sigaction do oblugi SIGTSTP
    struct sigaction act;
    act.sa_handler = reactSIGTSTP;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTSTP, &act, NULL);

    while(true) {
        printf("%c \n",letter);
        if(letter=='Z' && forward)       {  printf("\n"); letter = 'A'; }
        else if(letter=='A' && !forward) {  printf("\n"); letter = 'Z'; }
        else if(forward)                 {  letter++; }
        else if(!forward)                {  letter--; }

        // obluga SIGINT poprzed funkcje signal
        if(signal(SIGINT,reactSIGINT) == SIG_ERR) {
            printf("\n\nError while receiving signal SIGINT");
            exit(1);
        }

        usleep(250000);
    }
}