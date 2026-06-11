#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

#include "assembler.h"
#include "shared.c"

lineT TranslateInstruction(lineOg l){
    lineT Result = {
        .address = l.address
    };

    if(l.operand != 0){
        for(int i = 0; i < (int)sizeof(MemoryInstrTable) / (int)sizeof(TranslInfo); i++){
            if(!strcmp(l.name, MemoryInstrTable[i].name)){
                Result.parts.instr = MemoryInstrTable[i].number;
                Result.parts.opernd = l.operand;
                break;
            }
        }
        if(!strcmp(l.name, "DEC") || !strcmp(l.name, "HEX")){
            Result.instr = l.operand; // Just a number
        }
    }
    else{
        for(int i = 0; i < (int)sizeof(RegisterInstrTable) / (int)sizeof(TranslInfo); i++){
            if(!strcmp(l.name, RegisterInstrTable[i].name)){
                Result.instr = RegisterInstrTable[i].number;
                goto END;
            }
        }
        for(int i = 0; i < (int)sizeof(IoInstrTable) / (int)sizeof(TranslInfo); i++){
            if(!strcmp(l.name, IoInstrTable[i].name)){
                Result.instr = IoInstrTable[i].number;
                goto END;
            }
        }
    }

    END:
    return Result;
}

lineOg LoadLine(char *src){
    char TempBuffer[16]; int pos = 0;
    int srcPos = 0;

    while(src[pos] != '\000' && pos < 16){
        if(src[srcPos] == ';') {
            pos++;
            srcPos++;
            break;
        }
        
        TempBuffer[pos++] = src[srcPos];

        if(src[srcPos++] == '*'){
            TempBuffer[pos++] = ' ';
        }
    }
    TempBuffer[pos] = '\000';
    
    lineOg Result;
    char OperandBuffer[5];
    sscanf(TempBuffer, "%d %s %s", &Result.address, &Result.name, OperandBuffer);

    if(!strcmp(Result.name, "HEX")) Result.operand = (unsigned short)strtoul(TempBuffer, NULL, 16);
    else Result.operand = (unsigned short)strtoul(OperandBuffer, NULL, 10);

    for(int i = 0; Result.name[i] != '\000'; i++){
        if(Result.name[i] != '*')
            Result.name[i] &= 0xDF; // toUpper
    }

    return Result;
}

lineT *Assemble(char *str, size_t size, int *instrCount){
    int lCount = LineCount(str, size);
    lineT *tInstr = calloc(lCount, sizeof(lineT));
    int tInsCount = 0;
    if(tInstr == NULL)
        ErrorExit("Couldn't allocate space for instructions array");

    char *cPtr = strtok(str, "\n");
    lineOg instr = LoadLine(cPtr);

    while(cPtr != NULL){
        instr = LoadLine(cPtr);
        if(instr.name[0] != '\000'){
            tInstr[tInsCount] = TranslateInstruction(instr);
            tInsCount++;
        }
        cPtr = strtok(NULL, "\n");
    }

    *instrCount = tInsCount;
    return tInstr;
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, ".UTF8");

    if(argc != 2 && argc != 3){
        printf("Usage: %s <FilePath> [OutPath]\n"
                "If [OutPath] is not given then the program will create <FilePath>.bin", argv[0]);
        return 1;
    }
    
    FILE *f = fopen(argv[1], "r");
    if(f == NULL){
        ErrorExit("Couldn't open file");
    }

    char outPath[256];
    if(argc == 2){
        nameInfo ni = separateFileName(argv[1]);
        joinRoot(outPath, ni.path, ni.name);
        strcat(outPath, ".bin");
    }
    else{
        strcpy(outPath, argv[2]);
    }

    FILE *outF = fopen(outPath, "wb");
    if(outF == NULL){
        ErrorExit("Error creating output file");
    }

    long fSize = GetFileSize(f);

    char *FileBuffer = malloc((fSize+1) * sizeof(char));
    size_t Feedback = fread(FileBuffer, sizeof(char), fSize, f);
    FileBuffer[Feedback] = '\000';
    
    int InstrCount;
    lineT *instr = Assemble(FileBuffer, Feedback, &InstrCount);

    fwrite("", 1, 1, outF);
    fseek(outF, 0, SEEK_SET);

    for(int i = 0; i < InstrCount; i++){
        fseek(outF, instr[i].address*2, SEEK_SET);
        fwrite(&instr[i].instr, sizeof(unsigned short), 1, outF);
    }

    fclose(f);
    fclose(outF);
    free(instr);
    free(FileBuffer);
    return 0;
}