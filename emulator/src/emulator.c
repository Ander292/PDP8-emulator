#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <pthread.h>

#include "instrTable.h"
#include "emulator.h"
#include "shared.c"
#include "system.h"

pthread_mutex_t inputMutex;
pthread_mutex_t outputMutex;

word memory[MEMORY_SIZE];
/* Used to load the file before being moved to memory
   Depending on the type of loader used it will be interpreted differently */
word loadBuffer[MEMORY_SIZE];


int instrToStr(char *outBuffer, word memoryWord){
    switch(GET_TYPE(memoryWord)){
        case MEMORY_INSTRUCTION:
            for(word i = 0; i < sizeof(MemoryInstrTable) / sizeof(TranslInfo); i++){
                if(GET_INSTRUCTION(memoryWord) == MemoryInstrTable[i].number){
                    return sprintf(outBuffer, "%s %u", MemoryInstrTable[i].name, GET_OPERAND(memoryWord));
                }
            }
            goto ERROR;
        case REGISTER_INSTRUCTION:
            for(word i = 0; i < sizeof(RegisterInstrTable) / sizeof(TranslInfo); i++){
                if(memoryWord == RegisterInstrTable[i].number){
                    return sprintf(outBuffer, "%s", RegisterInstrTable[i].name);
                }
            }
            goto ERROR;
        case IO_INSTRUCTION:
            for(word i = 0; i < sizeof(IoInstrTable) / sizeof(TranslInfo); i++)
                if(memoryWord == IoInstrTable[i].number){
                    return sprintf(outBuffer, "%s", IoInstrTable[i].name);
                }
            goto ERROR;
        default:
            ERROR:
            outBuffer[0] = '?';
            outBuffer[1] = 0;
            return -1;
    }
}

int memoryDumpCsv(const char *outPath, const word *mem, size_t size){
    int errcode = 0;
    FILE *outF = fopen(outPath, "w");

    if(outF == NULL){
        perror("Fatel error opening out file");
        return -1;
    }

    fprintf(outF, "Address,Dec Value,Hex Value,Instruction\n");
    for(size_t i = 0; i < size; i++){
        char buffer[16];
        if(instrToStr(buffer, memory[i]) == -1) errcode++;
        fprintf(outF, "%llu,DEC %d,HEX %x,%s\n", i, memory[i], memory[i], buffer);
    }

    fclose(outF);
    return errcode;
}

int memoryDumpBin(const char *outPath, const word *mem, size_t size){
    FILE *outF = fopen(outPath, "wb");
    if(outF == NULL){
        perror("Fatel error opening out file");
        return -1;
    }

    fwrite(mem, sizeof(word), size, outF);
    fclose(outF);
    return 0;
}

int processArgs(int argc, char *argv[], char **outCsv, char **outBin, char **outPre){
    int result = 0;
    for(int i = 0; i < argc; i++){
        if(*argv[i] == '-'){
            switch(argv[i][1] | 0x20){
                case 'c':
                    *outCsv = argv[++i];
                    break;
                case 'b':
                    *outBin = argv[++i];
                    break;
                case 'p':
                    *outPre = argv[++i];
                    break;
                case 'n':
                    result = -1;
                    break;
                case 'd':
                    if(result == 0) result = 1;
                    break;
                default:
                    printf("Invalid flag: %s\n", argv[i]);
            }
        }
    }
    
    return result;
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, ".UTF8");

    if(argc < 3){
        printf("Usage: %s <FilePath> [-p <outPreCsv> -c <outCsv> -b <outBin> -n]\n"
                "-p <outPreCsv> is the path of the csv memory dump before the program starts, useful for seeing if the program is properly assembled\n"
                "-c <outCsv> is the path of the post run csv memory dump\n"
                "-b <outBin> is the path of the post run binary memory dump (the memory is written to a file in the same way it was kept internally)\n"
                "-n is passed to prevent the emulator from actually running the program (maybe useful if used together with -p flag)\n"
                "-d is passed to run in debug mode", argv[0]);
        return 1;
    }
    char *outCsv = NULL;
    char *outBin = NULL;
    char *outPre = NULL;

    int dontRun = processArgs(argc, argv, &outCsv, &outBin, &outPre);

    FILE *f = fopen(argv[1], "rb");
    if(f == NULL) ErrorExit("Fatal error opening bin file!");

    size_t size = fread(loadBuffer, sizeof(word), sizeof(loadBuffer)/sizeof(word), f);
    if(size == 0) ErrorExit("Size was 0");

    word startAddr = LOAD_PROGRAM(memory, loadBuffer);

    if(outPre != NULL) {
        int result = memoryDumpCsv(outPre, memory, MEMORY_SIZE);
        if(result != -1)
            printf("Wrote pre-run memDump(%d) to path: %s\n", result, outPre);
    }
    if(dontRun == -1) {
        printf("The emulator did not run.\n");
        return 0;
    }

    if(dontRun) 
        enterRawMode();
    registers regs = {0};

    initRegisters(&regs, startAddr);

// Creating the threads
    pthread_t processorTh, teleprinterOutTh, teleprinterInTh;

    processorArgs pArgs = {
        .debugMode = dontRun,
        .memory = memory,
        .regState = &regs
    };

    pthread_mutex_init(&inputMutex, NULL);
    pthread_mutex_init(&outputMutex, NULL);

    pthread_create(&teleprinterOutTh, NULL, &teleprinterOutputThread, (void*)&regs);
    pthread_create(&teleprinterInTh, NULL, &teleprinterInputThread, (void*)&regs);
    pthread_create(&processorTh, NULL, &processorThread, (void *)&pArgs);

    pthread_join(processorTh, NULL);
    pthread_join(teleprinterInTh, NULL);
    
    pthread_join(teleprinterOutTh, NULL);

    if(dontRun) 
        leaveRawMode();

    if(outBin != NULL) {
        if(!memoryDumpBin(outBin, memory, MEMORY_SIZE))
            printf("Wrote post-run binary memDump to path: %s\n", outBin);
    }
    if(outCsv != NULL) {
        int result = memoryDumpCsv(outCsv, memory, MEMORY_SIZE);
        if(result != -1)
            printf("Wrote post-run csv memDump(%d) to path: %s\n", result, outCsv);
    }

    return 0;
}