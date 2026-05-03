#include "main.h"

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

lineT TranslateInstruction(lineOg l){
    lineT Result = {
        .address = l.address
    };

    if(l.operand != 0){
        for(int i = 0; i < (int)sizeof(MemoryInstrTable); i++){
            if(!strcmp(l.name, MemoryInstrTable[i].name)){
                Result.instr = MemoryInstrTable[i].number << 12;
                Result.instr |= l.operand;
                break;
            }
        }
        if(!strcmp(l.name, "DEC") || !strcmp(l.name, "HEX")){
            Result.instr = l.operand; // Just a number
        }
    }
    else{
        for(int i = 0; i < (int)sizeof(RegisterTranslTable); i++){
            if(!strcmp(l.name, RegisterTranslTable[i].name)){
                Result.instr = RegisterTranslTable[i].number;
                break;
            }
        }
    }

    return Result;
}

lineOg LoadLine(char *src){
    char TempBuffer[16]; int pos = 0;

    while(src[pos] != '\000'){
        if(src[pos] == ';') {
            pos++;
            break;
        }
        TempBuffer[pos] = src[pos];
        pos++;
    }
    TempBuffer[pos] = '\000';
    puts(TempBuffer);
    
    lineOg Result;
    char OperandBuffer[5];
    sscanf(TempBuffer, "%d %s %s", &Result.address, &Result.name, OperandBuffer);

    if(!strcmp(Result.name, "HEX")) Result.operand = (unsigned short)strtoul(TempBuffer, NULL, 16);
    else Result.operand = (unsigned short)strtoul(OperandBuffer, NULL, 10);

    for(int i = 0; Result.name[i] != '\000'; i++){
        Result.name[i] &= 0xDF; // toUpper
    }

    return Result;
}

int Assemble(char *str, size_t size){
    int lCount = LineCount(str, size);
    lineOg *instructions = calloc(lCount, sizeof(lineOg));
    int insCount = 0;
    lineT *tInstr = calloc(lCount, sizeof(lineT));
    int tInsCount = 0;
    if(instructions == NULL) 
        ErrorExit("Couldn't allocate space for instructions array");

    char *cPtr = strtok(str, "\n");
    instructions[insCount] = LoadLine(cPtr);
    if(instructions[insCount].name[0] != '\0') insCount++;

    while((cPtr = strtok(NULL, "\n")) != NULL) {
        instructions[insCount] = LoadLine(cPtr);
        if(instructions[insCount].name[0] != '\0') insCount++;
    }

    for(int i = 0; i < insCount; i++){
        printf("Translating: %d|%s|%d\n", instructions[i].address, instructions[i].name, instructions[i].operand);
        tInstr[tInsCount] = TranslateInstruction(instructions[i]);
        printf("(%d)Result: %d\n", tInstr[tInsCount].address, tInstr[tInsCount].instr);
        tInsCount++;
    }

    free(instructions);
    free(tInstr);
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, ".UTF8");

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
    
    //fputs(FileBuffer, stdout);
    //putchar('\n');
    Assemble(FileBuffer, Feedback);
    
    free(FileBuffer);
    return 0;
}