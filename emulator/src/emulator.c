#include "instrTable.h"
#include "emulator.h"
#include "shared.c"
#include <string.h>
#include <stdio.h>
#include <locale.h>

typedef DEFINE_INSTR(instrFunction);

typedef struct instrInfo{
    instrFunction *fPtr;
    word instrId;
} instrInfo;

instrInfo MemoryInstr[] = {
    {and, AND},
    {add, ADD},
    {lda, LDA},
    {sta, STA},
    {bun, BUN},
    {bsa, BSA},
    {isz, ISZ}
};

instrInfo RegisterInstr[] = {
    {cla, CLA},
    {cle, CLE},
    {cma, CMA},
    {cme, CME},
    {cir, CIR},
    {cil, CIL},
    {inc, INC},
    {spa, SPA},
    {sna, SNA},
    {sza, SZA},
    {sze, SZE},
    {hlt, HLT}
};


word memory[MEMORY_SIZE];

registers regs = {0};

int instrToStr(char *outBuffer, word memoryWord){
    switch(GET_TYPE(memoryWord)){
        case MEMORY_INSTRUCTION:
            for(word i = 0; i < sizeof(MemoryInstrTable) / sizeof(TranslInfo); i++){
                if(GET_INSTRUCTION(memoryWord) == MemoryInstrTable[i].number){
                    return sprintf(outBuffer, "%s %u", MemoryInstrTable[i].name, GET_OPERAND(memoryWord));
                }
            }
            break;
        case REGISTER_INSTRUCTION:
            for(word i = 0; i < sizeof(RegisterInstrTable) / sizeof(TranslInfo); i++){
                if(memoryWord == RegisterInstrTable[i].number){
                    return sprintf(outBuffer, "%s", RegisterInstrTable[i].name);
                }
            }
            break;
        default:
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
        fprintf(outF, "%u,DEC %d,HEX %x,%s\n", i, memory[i], memory[i], buffer);
    }
    return errcode;
}

int memoryDumpBin(const char *outPath, const word *mem, size_t size){
    FILE *outF = fopen(outPath, "wb");
    if(outF == NULL){
        perror("Fatel error opening out file");
        return -1;
    }

    fwrite(mem, sizeof(word), size, outF);
}

int initRegisters(pRegisters regState, word startAddress){
    regState->PC = startAddress;
    regState->S = 1;
}

int main(int argc, char *argv[]){
    setlocale(LC_ALL, ".UTF8");

    if(argc != 3){
        printf("Usage: %s <FilePath> <StartingAddress>", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if(f == NULL) ErrorExit("Fatal error opening bin file!");

    size_t size = fread(memory, sizeof(word), sizeof(memory)/sizeof(word), f);
    if(size == 0) ErrorExit("Size was 0");

    initRegisters(&regs, atoi(argv[2]));

    memoryDumpCsv("assets/pre.csv", memory, MEMORY_SIZE);

    for(regs.SC = 0; regs.S == 1; regs.SC = (regs.SC + 1) % 4){
        switch(GET_CYCLE(regs.F, regs.R)){
            case CYCLE_FETCH:{
                switch(regs.SC){
                    case 0: // Preparing the address
                        regs.MAR = regs.PC;
                        break;
                    case 1: // Loading the address
                        regs.MBR = memory[regs.MAR];
                        regs.PC++; // Moving PC to the next instruction
                        break;
                    case 2: // Loading the instruction itself
                        word iopr = GET_INSTRUCTION(regs.MBR);
                        regs.I = (iopr & 0x8) >> 3;
                        regs.OPR = (iopr & 0x7);
                        break;
                    case 3:
                        /* Go to INDIRECT cycle if its a memory instruction with indirection flag on
                           Otherwise go to EXECUTE cycle */
                        if(regs.OPR != 7 && regs.I == 1) 
                            regs.R = 1; // INDIRECT
                        else regs.F = 1; // EXECUTE
                }
                /*
                    At the end of the EXECUTE cycle the address of the operand is inside MBR.
                    The instruction itself is inside I-OPR.
                    The operand itself needs to be loaded inside the EXECUTE cycle.

                    In case of indirection then the address of the memory word which contains the address of the operand is inside MBR[4-15].
                */
             } break;
            case CYCLE_INDIRECT:{
                switch(regs.SC){
                    case 0:
                        regs.MAR = GET_OPERAND(regs.MBR); // Address from MBR goes to MAR so it can be fetched
                        break;
                    case 1:
                        regs.MBR = memory[regs.MAR]; // Loading from memory
                        break;
                    case 2: break; // empty cycle
                    case 3: // Unconditionally move to EXECUTE cycle
                        regs.F = 1;
                        regs.R = 0;
                        break;
                }
            } break;
            case CYCLE_EXECUTE:{
                int found = 0;
                switch(GET_TYPE(regs.MBR)){
                    case MEMORY_INSTRUCTION:
                        for(word i = 0; i < sizeof(MemoryInstr) / sizeof(instrInfo); i++){
                            if(regs.OPR == MemoryInstr[i].instrId){
                                found = 1;
                                MemoryInstr[i].fPtr(&regs);
                                break;
                            }
                        }
                        if(!found){
                            printf("Invalid isntruction at PC = %d!\n", regs.PC-1);
                            regs.F = 0;
                            regs.R = 0;
                        }
                        break;
                    case REGISTER_INSTRUCTION:
                        for(word i = 0; i < sizeof(RegisterInstr) / sizeof(instrInfo); i++){
                            if(regs.MBR == RegisterInstr[i].instrId){
                                found = 1;
                                RegisterInstr[i].fPtr(&regs);
                                break;
                            }
                        }
                        if(!found){
                            printf("Invalid isntruction at PC = %d!\n", regs.PC-1);
                            regs.F = 0;
                            regs.R = 0;
                        }
                        break;
                    default:
                        ErrorExit("Input output not implemented yet!");
                }
            } break;
            case CYCLE_INTERRUPT:{
                ErrorExit("Input/Output isnt implemented yet");
                switch(regs.SC){
                    case 0: // Saving the old PC and zeroing PC
                        regs.MBR = regs.PC;
                        regs.PC = 0;
                        break;
                    case 1: // Setting MAR to zero and incrementing PC to one
                        regs.MAR = 0;
                        regs.PC++;
                        break;
                    case 2: // Saving old PC at address 0 and disabling interrupts
                        memory[0] = regs.MBR;
                        regs.IEN = 0;
                        break;
                    case 3: // Going to FETCH cycle (will now start from address 1)
                        regs.F = 0;
                        regs.R = 0;
                        break;
                }
            } break;
        }
    }

    memoryDumpBin("assets/memDump.bin", memory, MEMORY_SIZE);
    memoryDumpCsv("assets/memDump.csv", memory, MEMORY_SIZE);
    
    return 0;
}