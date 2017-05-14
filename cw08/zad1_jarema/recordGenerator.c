#include "general.h"

void rand_str(char *dest, size_t length) {
    char charset[] =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

int main(int argc, char *argv[]){
    FILE *fp = fopen("records.dat", "wb");
    if (!fp){
        printf("Cannot create file\n");
        return 1;
    }

    if(argc != 2){
        printf("Bad amount of arguments\n");
        return 1;
    }

    int rec = atoi(argv[1]);

    char * record = (char *)malloc(BUFFERSIZE * sizeof(char));

    for(int i=0; i<rec; i++){
        rand_str(record,BUFFERSIZE-MAXIDDIGITS-1);
        fprintf(fp, "%05d%s\n", i+1, record);
    }

    free(record);
    fclose(fp);

    return 0;
}