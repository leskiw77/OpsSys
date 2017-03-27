#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Invalid amount of arguments\n");
        return 1;
    }

    char * s = getenv(argv[1]);

    if(s == NULL){
        printf("No such environmental variable\n");
    }else{
        printf("%s\n",s);
    }
    return 0;
}