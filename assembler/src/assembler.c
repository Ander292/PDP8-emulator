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
    
    char instr[16]; int iPos = 0;
    char oprnd[16];

    for(int i = 0; instrStr[i] != '\000'; i++){
        char c = 0;
        if(IS_LETTER(instrStr[i])) c = instrStr[i] & 0xDF; // toUpper
        else if(instrStr[i] == '*') {
            instr[iPos++] = instrStr[i];
            instr[iPos++] = ' ';
            continue;
        }
        else c = instrStr[i];

        instr[iPos++] = c;
    }
    instr[iPos] = 0;
    
    if(instrStr[3] == '*'){
        //strncpy(instr, instrStr, 4);
        // instr[3] = ' ';
        // instr[4] = 0;
        strcpy(oprnd, instrStr + 4);
        instr[4] = 0;
    }
    else{
        // strcpy(instr, instrStr);
        strcpy(oprnd, operandStr);
    }

    if(oprnd == NULL){
        // Memory or register
        for(unsigned int i = 0; i < sizeof(RegisterInstrTable) / sizeof(TranslInfo); i++){
            if(!strcmp(instr, RegisterInstrTable[i].name)){
                *outCode = RegisterInstrTable[i].number;
                *outType = REGISTER_INSTRUCTION;
                *outOperand = 0;
                goto END;
            }
        }
        for(unsigned int i = 0; i < sizeof(IoInstrTable) / sizeof(TranslInfo); i++){
            if(!strcmp(instr, IoInstrTable[i].name)){
                *outCode = IoInstrTable[i].number;
                *outType = IO_INSTRUCTION;
                *outOperand = 0;
                goto END;
            }
        }
        goto ERROR; // Didnt find it
    }else{
        for(unsigned int i = 0; i < sizeof(MemoryInstrTable) / sizeof(TranslInfo); i++){
            if(!strcmp(instr, MemoryInstrTable[i].name)){
                *outCode = MemoryInstrTable[i].number;
                *outOperand = strtoul(oprnd, NULL, convMode);
                *outType = MEMORY_INSTRUCTION;
                goto END;
            }
        }
        goto ERROR; // Didnt find it
    }

    ERROR:
        printf("Invalid instruction: %s | %s ", instrStr, operandStr);
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
            return (lineT){.address = 0, .opCode = 0, .operand = 0, .type = TRANSLATION_SKIP };
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

    if(translInstr(instrName, operandBuffer, &result.opCode, &result.type, &result.operand)){
        printf("(%d)\n", result.address);
    }

    return result;
}

word lineToBin(lineT src){
    word result = 0;
    switch(src.type){
        case MEMORY_INSTRUCTION:
            result = (src.opCode << 12) | src.operand;
            break;
        case REGISTER_INSTRUCTION:
            result = src.opCode;
            break;
        case IO_INSTRUCTION:
            result = src.opCode;
            break;
        default:
            printf("Fatal error, non instruction sent for translation: (%d) %d %d %d\n", 
                src.address, src.type, src.opCode, src.operand);
            break;
    }

    return result;
}

size_t assemble(char *source, word *out, size_t inSize){
    size_t result = 0;

    int lCount = LineCount(source, inSize);
    lineT *instrArr = malloc(sizeof(lineT) * lCount);
    int pos = 0;

    char *focus = strtok(source, "\r\n");
    while(focus != NULL){
        instrArr[pos++] = splitLine(focus);

        focus = strtok(NULL, "\r\n");
    }
    

    for(int i = 0; i < lCount; i++){
        out[result] = lineToBin(instrArr[i]);
        result += 2;
    }

    return result;
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

    size_t finalSize = assemble(fileBuffer, binary, feedback);

    fclose(outF);
    free(fileBuffer);

    return 0;
}