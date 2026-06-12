#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

#include "assembler.h"
#include "shared.h"
#include "shared.c"
#include "lineList.h"
#include "instrTable.h"

#undef DEBUG

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

int contains(const char *flagC, char c){
    for(; *flagC != 0; flagC++)
        if(*flagC == c) return 1;
    
    return 0;
}

char *tokenize(char *str, const char *del, char **out){
    if(str == NULL) return NULL;
    
    char *result = str;
    for(;*str != 0 && contains(del, *str); str++);
    if(*str == 0) return NULL;

    for(char *sPtr = str;; sPtr++){
        if(contains(del, *sPtr)){
            *out = sPtr + 1;
            result = str;
            *sPtr = 0;
            goto END;
        }
        else if(*sPtr == 0){
            *out = NULL;
            result = str;
            goto END;
        }
    }

    END:
    return result;
}

/*
    Phases of assembling:

    1) Splitting the input into lines. Tokenizing each line into lineOg structure.
        - Removing comments and stuff
    2) Translating the instructions and determining their type
        - Getting rid of non-instruction directives
    3) Writting it to a file in 3 formats
        - Raw loading (exact memory state)
        - Absolute loader
        - Relative loader
*/

lineOg extractLine(char *line){
    char buffer[64];
    int pos = 0;
    for(; *line != 0; line++){
        if(*line == ';') break;
        else if(*line == '*' && *(line+1) != ' '){
            buffer[pos++] = '*';
            buffer[pos++] = ' ';
            continue;
        }

        buffer[pos++] = *line;
    }
    buffer[pos] = 0;
    if(*buffer == 0) return (lineOg){0, {0}, 0};

    /*
        Extracting parts manually
    */
    char *contPtr = NULL;
    char *addr = tokenize(buffer, " ", &contPtr);
    char *instr = tokenize(contPtr, " ", &contPtr);
    char *oprnd = tokenize(contPtr, " ", &contPtr);

    //printf("%s|%s|%s\n", addr, instr, oprnd);
    lineOg result;
    result.address = strtoul(addr, NULL, convMode);
    if(!strcmp("HEX", result.name))
        result.operand = strtoul(oprnd, NULL, 16);
    else if(!strcmp("DEC", result.name))
        result.operand = strtoul(oprnd, NULL, 10);
    else
        result.operand = strtoul(oprnd, NULL, convMode);

    strcpy(result.name, instr);

    return result;
}
lineT translateLine(lineOg og){
    lineT result;
    result.address = og.address;
    result.operand = og.operand;
    
    /* Memory instructions have operand */
    for(unsigned int i = 0; i < sizeof(MemoryInstrTable) / sizeof(TranslInfo); i++){
        if(!strcmp(MemoryInstrTable[i].name, og.name)){
            result.opCode = MemoryInstrTable[i].number;
            result.type = MEMORY_INSTRUCTION;
            goto END;
        }
    }
    /* Register and input-output instructions have no operand */
    for(unsigned int i = 0; i < sizeof(RegisterInstrTable) / sizeof(TranslInfo); i++){
        if(!strcmp(RegisterInstrTable[i].name, og.name)){
            result.opCode = RegisterInstrTable[i].number;
            result.type = REGISTER_INSTRUCTION;
            goto END;
        }
    }
    for(unsigned int i = 0; i < sizeof(IoInstrTable) / sizeof(TranslInfo); i++){
        if(!strcmp(IoInstrTable[i].name, og.name)){
            result.opCode = IoInstrTable[i].number;
            result.type = IO_INSTRUCTION;
            goto END;
        }
    }

    if(!strcmp(og.name, "DEC")){
        result.opCode = DEC_M;
        result.operand = og.operand;
        result.type = TRANSLATION_MACRO;
        goto END;
    }
        
    else if(!strcmp(og.name, "HEX")){
        result.opCode = HEX_M;
        result.operand = og.operand;
        result.type = TRANSLATION_MACRO;
        goto END;
    }
    
    printf("Error translating instruction: %6u | %4s | %6u\n", og.address, og.name, og.operand);
    result.type = TRANSLATION_SKIP; /* Error while translating instructions (it was not found) */

    END:
    return result;
}
size_t assemble(char *raw, word *bin, size_t size){
    size_t result = 0;
    /* Tokenizing the input string and creating a linked list of
       all lines */
    lineList listOg = listCreate();
    char *remainder = NULL;
    char *out = tokenize(raw, "\n", &remainder);
    while(out != NULL){        
        lineOg line = extractLine(out);
        if(*(line.name) != 0) insertEndOg(&listOg, line);
        out = tokenize(remainder, "\n", &remainder);
    }

    /* Translating line by line into machine code */
    lineList listT = listCreate();
    
    lineOg line = deleteFrontOg(&listOg);
    while(*(line.name) != 0){
        lineT transltd = translateLine(line);
        if(transltd.type != TRANSLATION_SKIP) insertEndT(&listT, transltd);

        line = deleteFrontOg(&listOg);
    }
    

    //printList(&listT);

    lineT translLine = deleteFrontT(&listT);
    while(translLine.type != TRANSLATION_SKIP){
        result += 2;
        word finalWord = 0;
        switch(translLine.type){
            case MEMORY_INSTRUCTION:
                finalWord = (translLine.opCode << 12) | (translLine.operand);
                break;
            case REGISTER_INSTRUCTION:
            case IO_INSTRUCTION:
                finalWord = translLine.opCode;
                break;
            case TRANSLATION_MACRO:
                if(translLine.opCode == DEC_M || translLine.opCode == HEX_M)
                    finalWord = translLine.operand;
                else printf("Invalid macro\n");
                break;
            default:
                printf("Invalid type\n");
        }
        binary[translLine.address] = finalWord;
        translLine = deleteFrontT(&listT);
    }

    freeList(&listT);
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

    /*
        Might be larger than the actually inputed string because
        of windows newlines being converted on read
    */
    long fSize = GetFileSize(inF);

    char *fileBuffer = malloc((fSize+1) * sizeof(char));
    size_t feedback = fread(fileBuffer, sizeof(char), fSize, inF);
    fileBuffer[feedback] = '\000';
    fclose(inF);

#ifndef DEBUG
    size_t finalSize = assemble(fileBuffer, binary, feedback);

    fwrite(binary, sizeof(word), MEMORY_SIZE, outF);
#else // WIP, TESTING
    #if 0
        lineList l = listCreate();
        lineT l1 = (lineT){1, 200, 15, 300};
        lineT l2 = (lineT){2, 201, 15, 300};
        lineT l3 = (lineT){1, 200, 15, 350};
        insertEndT(&l, l1);
        insertEndT(&l, l2);
        insertEndT(&l, l3);
        lineOg o1 = (lineOg){200, "DEC", 0};
        lineOg o2 = (lineOg){214, "LDA", 350};
        lineOg o3 = (lineOg){212, "BUN*", 1405};
        insertEndOg(&l, o1);
        insertEndOg(&l, o2);
        insertEndOg(&l, o3);
        
        printList(&l);
    #endif
    //printf("%s|%s|%s\n", out1, out2, out3);
#endif  



    fclose(outF);
    free(fileBuffer);

    return 0;
}