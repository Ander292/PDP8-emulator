#include "emulator.h"
#include "system.h"

#include <string.h>
#include <stdio.h>
#include <pthread.h>

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

instrInfo InOutInstr[] = {
    {inp, INP},
    {out, OUT},
    {ski, SKI},
    {sko, SKO},
    {ion, ION},
    {iof, IOF}
};

void initRegisters(pRegisters regState, word startAddress){
    regState->PC = startAddress;
    regState->S = 1;
}

void formatLimiter(char *buffer, char c, int size){
    buffer[0] = '|';
    for(int i = 1; i < size-1; i++){
        buffer[i] = c;
    }
    buffer[size-1] = '|';
}

void formatAndDisplayOutput(registers *regState, registers *oldState, word *memory){
    char buffer[256];
    /*
        Here the register and memory info is formated and printed
    */

    char instrName[32];
    word cycle = (regState->SC == 3 ? 
        GET_CYCLE(oldState->F, oldState->R) : 
        GET_CYCLE(regState->F, regState->R));
    instrToStr(instrName, memory[regState->PC-1]);
    sprintf(buffer,
        ESC_CLEAR_LINE"c%d t%d: %s\n"
        ESC_CLEAR_LINE"PC:    %-4d\tCurrent: %s\n"
        ESC_CLEAR_LINE"ACC:   %-8d\tE: %1d\n"
        ESC_CLEAR_LINE"MBR:   %-8d\n"
        ESC_CLEAR_LINE"IOPR: %s%-2d\tMAR %-8d",
        cycle, regState->SC, GET_CYCLE_STRING(cycle),
        regState->PC, instrName,
        regState->ACC, regState->E, regState->MBR,
        (regState->I ? "I" : " "), regState->OPR, regState->MAR
    );

    puts(ESC_RESET_CURSOR_POS);
    puts(buffer);
    puts(ESC_RESET_CURSOR_POS);
    fflush(stdout);
}

void processor(registers *regState, word *memory, int debugMode){
    int step = ' ';
    registers oldState = {0};
    for(regState->SC = 0; regState->S == 1; regState->SC = (regState->SC + 1) % 4){
        // sleepF(1);
        switch(GET_CYCLE(regState->F, regState->R)){
            case CYCLE_FETCH:{
                switch(regState->SC){
                    case 0: // Preparing the address
                        regState->MAR = regState->PC;
                        break;
                    case 1: // Loading the address
                        regState->MBR = memory[regState->MAR];
                        regState->PC++; // Moving PC to the next instruction
                        break;
                    case 2: // Loading the instruction itself
                        word iopr = GET_INSTRUCTION(regState->MBR);
                        regState->I = (iopr & 0x8) >> 3;
                        regState->OPR = (iopr & 0x7);
                        break;
                    case 3:
                        /* Go to INDIRECT cycle if its a memory instruction with indirection flag on
                           Otherwise go to EXECUTE cycle */
                        if(regState->OPR != 7 && regState->I == 1) 
                            regState->R = 1; // INDIRECT
                        else regState->F = 1; // EXECUTE
                }
                /*
                    At the end of the EXECUTE cycle the address of the operand is inside MBR.
                    The instruction itself is inside I-OPR.
                    The operand itself needs to be loaded inside the EXECUTE cycle.

                    In case of indirection then the address of the memory word which contains the address of the operand is inside MBR[4-15].
                */
             } break;
            case CYCLE_INDIRECT:{
                switch(regState->SC){
                    case 0:
                        regState->MAR = GET_OPERAND(regState->MBR); // Address from MBR goes to MAR so it can be fetched
                        break;
                    case 1:
                        regState->MBR = memory[regState->MAR]; // Loading from memory
                        break;
                    case 2: break; // empty cycle
                    case 3: // Unconditionally move to EXECUTE cycle
                        regState->F = 1;
                        regState->R = 0;
                        break;
                }
            } break;
            case CYCLE_EXECUTE:{
                int found = 0;
                switch(GET_TYPE(regState->MBR)){
                    case MEMORY_INSTRUCTION:
                        for(word i = 0; i < sizeof(MemoryInstr) / sizeof(instrInfo); i++){
                            if(regState->OPR == MemoryInstr[i].instrId){
                                found = 1;
                                MemoryInstr[i].fPtr(regState);
                                break;
                            }
                        }
                        if(!found){
                            printf("Invalid isntruction at PC = %d!\n", regState->PC-1);
                            regState->F = 0;
                            regState->R = 0;
                        }
                        break;
                    case REGISTER_INSTRUCTION:
                        for(word i = 0; i < sizeof(RegisterInstr) / sizeof(instrInfo); i++){
                            if(regState->MBR == RegisterInstr[i].instrId){
                                found = 1;
                                RegisterInstr[i].fPtr(regState);
                                break;
                            }
                        }
                        if(!found){
                            printf("Invalid isntruction at PC = %d!\n", regState->PC-1);
                            regState->F = 0;
                            regState->R = 0;
                        }
                        break;
                    case IO_INSTRUCTION:
                        for(word i = 0; i < sizeof(InOutInstr) / sizeof(instrInfo); i++){
                            if(regState->MBR == InOutInstr[i].instrId){
                                found = 1;
                                InOutInstr[i].fPtr(regState);
                                break;
                            }
                        }
                        if(!found){
                            printf("Invalid isntruction at PC = %d!\n", regState->PC-1);
                            regState->F = 0;
                            regState->R = 0;
                        }
                        break;
                    default:
                        char buffer[64];
                        sprintf(buffer, "Fatal error at address %d!\n", regState->PC-1);
                        ErrorExit(buffer);
                }
            } break;
            case CYCLE_INTERRUPT:{
                ErrorExit("Input/Output isnt implemented yet");
                switch(regState->SC){
                    case 0: // Saving the old PC and zeroing PC
                        regState->MBR = regState->PC;
                        regState->PC = 0;
                        break;
                    case 1: // Setting MAR to zero and incrementing PC to one
                        regState->MAR = 0;
                        regState->PC++;
                        break;
                    case 2: // Saving old PC at address 0 and disabling interrupts
                        memory[0] = regState->MBR;
                        regState->IEN = 0;
                        break;
                    case 3: // Going to FETCH cycle (will now start from address 1)
                        regState->F = 0;
                        regState->R = 0;
                        break;
                }
            } break;
        }
        if(debugMode){
            while((step != ' ')) {
                step = getchar();
                if((step | 0x20) == 's' || (step | 0x20) == 'c' || (step | 0x20) == 'p')
                    goto END;
            }
            step = 0;
            //if(GET_CYCLE(regState->F, regState->R) == CYCLE_FETCH && regState->SC == 0) 
                //printf("%d\n", regState->PC);
            formatAndDisplayOutput(regState, &oldState, memory);
        }
        // If output flag is 0, start the teleprinter
        oldState = *regState;
    }
    END:
    if(debugMode) puts(ESC_CLEAR_SCREEN);
    //puts("Processor is about to stop...!");
    if(debugMode) getchar();
}

void *processorThread(void *args){
    //puts("Processor started");
    processorArgs *pArgs = ((processorArgs *)args);
    processor(pArgs->regState, pArgs->memory, pArgs->debugMode);
    return NULL;
}