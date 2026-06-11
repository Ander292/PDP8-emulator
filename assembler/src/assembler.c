#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

#include "assembler.h"
#include "shared.h"
#include "shared.c"

#define TEMP_BUFFER_SIZE 32
#define MAX_BINARY_SIZE (MEMORY_SIZE*2)

#define CONV_MODE_DECIMAL 10
#define CONV_MDOE_HEX 16
/*
    Contains the final binary data that is in the end written to a file.
    TODO: Maybe use a memory mapped file (will require platform specific stuff)
*/
word binary[MAX_BINARY_SIZE] = {0};
int convMode = CONV_MODE_DECIMAL;

int translInstr(char *instrStr, char *operandStr, word *outCode, word *outType, word *outOperand){  
    if(instrStr == NULL || outCode == NULL || outType == NULL || outOperand == NULL)
        return -1; // If any arg is null, return error
    
    char instr[8];
    char oprnd[8];

    for(int i = 0; instrStr[i] != '\000'; i++){
        if(instrStr[i] != '*')
            instr[i] = instrStr[i] & 0xDF; // toUpper
    }

    if(instrStr[3] == '*'){
        //strncpy(instr, instrStr, 4);
        instr[4] = ' ';
        instr[5] = 0;
        strcpy(oprnd, instrStr + 4);
    }
    else{
        strcpy(instr, instrStr);
        strcpy(oprnd, operandStr);
    }

    if(oprnd == NULL){
        // Memory or register
        for(int i = 0; i < sizeof(RegisterInstrTable) / sizeof(TranslInfo); i++){
            if(!strcmp(instr, RegisterInstrTable[i].name)){
                *outCode = RegisterInstrTable[i].number;
                *outType = REGISTER_INSTRUCTION;
                goto END;
            }
        }
        for(int i = 0; i < sizeof(IoInstrTable) / sizeof(TranslInfo); i++){
            if(!strcmp(instr, IoInstrTable[i].name)){
                *outCode = IoInstrTable[i].number;
                *outType = IO_INSTRUCTION;
                goto END;
            }
        }
        goto ERROR; // Didnt find it
    }else{
        for(int i = 0; i < sizeof(MemoryInstrTable) / sizeof(TranslInfo); i++){
            if(!strcmp(instr, MemoryInstrTable[i].name)){
                *outCode = MemoryInstrTable[i].number;
                *outOperand = strtoul(operandStr, NULL, convMode);
                *outType = MEMORY_INSTRUCTION;
                goto END;
            }
        }
        goto ERROR; // Didnt find it
    }

    ERROR:
        printf("Didn't find the instruction: %s | %s\n", instrStr, operandStr);
        return 1;
    END:
        return 0;
}

/* Translates a line into a lineOg struct */
lineT splitLine(char *src){
    char tempBuffer[TEMP_BUFFER_SIZE]; int pos = 0;
    int srcPos = 0;

    while(src[pos] != 0 && pos < TEMP_BUFFER_SIZE){
        if(src[srcPos] == ';'){
            pos++;
            srcPos++;
            break;
        }

        tempBuffer[pos++] = src[srcPos];

        if(src[srcPos++] == '*'){
            tempBuffer[pos++] = ' ';
        }
    }
    tempBuffer[pos] = '\000';

    lineT result;
    // char operandBuffer[8];
    // char instrName[8];
    
    char *adrBuffer = strtok(tempBuffer, " ");
    char *instrName = strtok(NULL, " ");
    char *operandBuffer = strtok(NULL, " ");

    result.address = strtoul(tempBuffer, NULL, convMode);
    translInstr(instrName, operandBuffer, &result.opCode, &result.type, &result.operand);

    return result;
}

#if 0
lineT translInstr(lineOg l){
    lineT result = {
        .address = l.address
    };

    if(l.operand != 0){
        for(int i = 0; i < (int)sizeof(MemoryInstrTable) / (int)sizeof(TranslInfo); i++){
            if(!strcmp(l.name, MemoryInstrTable[i].name)){
                result.parts.instr = MemoryInstrTable[i].number;
                result.parts.opernd = l.operand;
                break;
            }
        }
        if(!strcmp(l.name, "DEC") || !strcmp(l.name, "HEX")){
            result.instr = l.operand; // Just a number
        }
    }
    else{
        for(int i = 0; i < (int)sizeof(RegisterInstrTable) / (int)sizeof(TranslInfo); i++){
            if(!strcmp(l.name, RegisterInstrTable[i].name)){
                result.instr = RegisterInstrTable[i].number;
                goto END;
            }
        }
        for(int i = 0; i < (int)sizeof(IoInstrTable) / (int)sizeof(TranslInfo); i++){
            if(!strcmp(l.name, IoInstrTable[i].name)){
                result.instr = IoInstrTable[i].number;
                goto END;
            }
        }
    }

    END:
    return result;
}
#endif

size_t assemble(char *source, word *out, size_t inSize){
    size_t result = 0;

    char *test = "250 HEX c";
    lineT split = splitLine(test);
    //printf("%d %s %d", split.address, split.name, split.operand);
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, ".UTF8");

    if(argc != 2 && argc != 3){
        printf("Usage: %s <FilePath> [OutPath]\n"
                "If [OutPath] is not given then the program will create <FilePath>.bin", argv[0]);
        return 1;
    }

    // Opening input file
    FILE *inF = fopen(argv[1], "r");
    if(inF == NULL){
        ErrorExit("Error opening input file");
    }

    /* If the out file is given as an arg then write to it. If not then create
       a file of same name as input file but with .bin extension */
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
        ErrorExit("Error opening output file");
    }

    long fSize = GetFileSize(inF);

    char *fileBuffer = malloc((fSize+1) * sizeof(char));
    size_t feedback = fread(fileBuffer, sizeof(char), fSize, inF);
    fileBuffer[feedback] = '\000';
    fclose(inF);

    //size_t finalSize = assemble(fileBuffer, binary, feedback);


    word outCode;
    word outType;
    word outOperand;

    int n = translInstr("BUN*300", NULL, &outCode, &outType, &outOperand);

    printf("%d %d %d %d", n, outCode, outType, outOperand);

    fclose(outF);
    free(fileBuffer);

    return 0;
}