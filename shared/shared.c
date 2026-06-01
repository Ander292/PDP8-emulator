#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M_FORW 1
#define M_BACK 2

#define joinRoot(string_dest, string_root, string_extra) strcpy(string_dest, string_root); joinPath(string_dest, string_extra, 1);

typedef struct nameInfo{
    char path[128];
    char name[64];
    char extension[16];
    int type; // 1--file, 2--dir
} nameInfo;

nameInfo separateFileName(const char *str){
    int pExt = 0;
    int pName = 0;

    int pos, len;
    pos = len = strlen(str);
    nameInfo fInfo = {0};
    
    while(pos > 0) {
        if(str[pos] == '.' && !pExt) {
            pExt = pos;
            memcpy(fInfo.extension, str + pos, len - pos);
            fInfo.extension[len-pos] = '\0';
        }
        else if((str[pos] == '/' || str[pos] == '\\') && !pName) {
            if(pExt) {
                memcpy(fInfo.name, str + pos + 1, pExt - (pos + 1));
                fInfo.extension[pExt - (pos + 1)] = '\0';
            }
            else {
                memcpy(fInfo.name, str + pos + 1, len - (pos + 1));
                fInfo.extension[len - (pos + 1)] = '\0';
            }

            memcpy(fInfo.path, str, pos + 1);
            fInfo.extension[pos] = '\0';

            pName = pos;
        }

        pos--;
        fInfo.type = 1;
    }

    if(!pName) {
        if(pExt)
            memcpy(fInfo.name, str, pExt);
        else
            memcpy(fInfo.name, str, len);

        *(fInfo.path) = 0;
    }
    
    return fInfo;
}

void mergeFileName(nameInfo fInfo, char *out){
    strcat(out, fInfo.path);
    strcat(out, fInfo.name);
    strcat(out, fInfo.extension);
}

char *joinPath(char *dest, const char *src, int mode){
    int lenDest = strlen(dest);
    if(*src == '\\' || *src == '/') src += 1;

    if(mode == M_FORW)
        if(dest[lenDest - 1] == '/'){
            strcat(dest, src);
        }
        else{
            strcat(dest, "/");
            strcat(dest, src);
        }
    else if(mode == M_BACK)
        if(dest[lenDest - 1] == '\\'){
            strcat(dest, src);
        }
        else{
            strcat(dest, "\\");
            strcat(dest, src);
        }
    return dest;
}


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

