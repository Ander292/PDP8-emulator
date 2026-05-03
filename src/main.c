#include "main.h"

void ErrorExit(char *string){
    char *errorString = strerror(errno);
    printf("%s : %s\n", string, errorString);
    exit(1);
}

long GetFileSize(FILE *f){
    long fPos = ftell(f);
    fseek(f, 0, SEEK_END);

    long Result  = ftell(f);
    fseek(f, fPos, SEEK_SET);

    return Result;
}

int main(int argc, char *argv[]){
    if(argc != 2) {
        printf("Usage: %s <FilePath>", argv[0]);
        return 1;
    }
    
    FILE *f = fopen(argv[1], "r");
    if(f == NULL){
        ErrorExit("Couldn't open file");
    }

    long fSize = GetFileSize(f);

    char *FileBuffer = malloc((fSize+1) * sizeof(char));
    size_t Feedback = fread(FileBuffer, sizeof(char), fSize, f);
    FileBuffer[Feedback] = '\000';
    
    fputs(FileBuffer, stdout);
    free(FileBuffer);

    return 0;
}