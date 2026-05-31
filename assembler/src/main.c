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
    //puts(TempBuffer);
    
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

lineT *Assemble(char *str, size_t size, int *instrCount){
    int lCount = LineCount(str, size);
    lineT *tInstr = calloc(lCount, sizeof(lineT));
    int tInsCount = 0;
    if(tInstr == NULL)
        ErrorExit("Couldn't allocate space for instructions array");

    char *cPtr = strtok(str, "\n");
    lineOg instr = LoadLine(cPtr);
    if(instr.name[0] != '\000'){
        fprintf(stdout, "Translating: %d|%s|%d\n", instr.address, instr.name, instr.operand);
        tInstr[tInsCount] = TranslateInstruction(instr);
        fprintf(stdout, "(%d)Result: %d\n", tInstr[tInsCount].address, tInstr[tInsCount].instr);
        tInsCount++;
    }

    while((cPtr = strtok(NULL, "\n")) != NULL){
        instr = LoadLine(cPtr);
        if(instr.name[0] != '\000'){
            fprintf(stdout, "Translating: %d|%s|%d\n", instr.address, instr.name, instr.operand);
            tInstr[tInsCount] = TranslateInstruction(instr);
            fprintf(stdout, "(%d)Result: %d\n", tInstr[tInsCount].address, tInstr[tInsCount].instr);
            tInsCount++;
        }
    }

    *instrCount = tInsCount;
    return tInstr;
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, ".UTF8");

    if(argc != 2){
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
    
    int InstrCount;
    lineT *instr = Assemble(FileBuffer, Feedback, &InstrCount);

    FILE *outF = fopen("assets/out.bin", "wb");
    fwrite("", 1, 1, outF);
    fseek(outF, 0, SEEK_SET);

    //fwrite(intr, sizeof(lineT), InstrCount, outF);
    for(int i = 0; i < InstrCount; i++){
        printf("Address: (%d|%x) Instruction: (%d|%x)\n", instr[i].address, instr[i].address, instr[i].instr, instr[i].instr);
        fseek(outF, instr[i].address*2, SEEK_SET);
        fwrite(&instr[i].instr, sizeof(unsigned short), 1, outF);
    }

    fclose(f);
    fclose(outF);
    free(instr);
    free(FileBuffer);
    return 0;
}