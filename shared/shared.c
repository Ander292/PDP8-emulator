#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ErrorExit(char *str){
    char *errorString = strerror(errno);
    printf("%s : %s\n", str, errorString);
    exit(1);
}

long GetFileSize(FILE *f){
    long fPos = ftell(f);
    fseek(f, 0, SEEK_END);

    long Result  = ftell(f);
    fseek(f, fPos, SEEK_SET);

    return Result;
}

int LineCount(char *str, size_t size){
    int Result = 1;
    for(size_t i = 0; i < size; i++){
        if(str[i] == '\n') Result += 1;
    }

    return Result;
}