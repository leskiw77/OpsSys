#include "commands.h"

int main(int argc, char *argv[]) {
    char *line = NULL;
    int size = 0;
    printf("Pipe interpreter\n");
    while (1) {
        // read line from stdin :
        printf("Command > \n");
        int readSigns = getline(&line, &size, stdin);
        if (readSigns > 1) {
            executeLine(line, readSigns);
        } else {
            printf("End of interprer's work\n");
            free(line);
            break;
        }
    }
    return 0;
}

