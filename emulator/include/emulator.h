#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h> // For typedefs

#define MEMORY_SIZE 4096
#define WORD_SIZE 2

#define MEMORY_INSTRUCTION 0
#define REGISTER_INSTRUCTION 1
#define IO_INSTRUCTION 2

#define GET_INSTRUCTION(memWord) (((instr)((word)memWord)).parts.instr)
#define GET_OPERAND(memWord) (((instr)((word)memWord)).parts.opernd)
#define GET_TYPE(memWord) ((GET_INSTRUCTION(memWord) & 0x7) != 7 ? MEMORY_INSTRUCTION : (GET_INSTRUCTION(memWord) & 0x8 ? IO_INSTRUCTION : REGISTER_INSTRUCTION))

#define GET_CYCLE(f, r) (((f) << 1) + (r))
#define CYCLE_FETCH 0
#define CYCLE_INDIRECT 1
#define CYCLE_EXECUTE 2
#define CYCLE_INTERRUPT 3

#define GET_CYCLE_STRING(n) ((n == CYCLE_FETCH ? "FETCH" : (n == CYCLE_INDIRECT ? "INDIRECT" : (n == CYCLE_EXECUTE ? "EXECUTE" : "INTERRUPT"))))

#include "instr.h"

//----------------------------------------------------------------------//

typedef uint16_t word;
typedef uint8_t byte;

typedef union instr{
    word val;
    struct{
        word opernd : 12;
        word instr : 4;
    } parts;
} instr;

typedef struct registers{
    word PC : 12;
    struct{
        word MAR : 12;
        word OPR : 3;
        word I : 1;
    };
    word MBR;
    word ACC;
    struct{
        word E : 1; // Overflow
        word S : 1; // Enable bit
        word SC : 2; // Sequence counter
        word F : 1; //
        word R : 1; // FR cycle
        

        word IEN : 1; // Interrupt
        word FGI : 1; // Input ready
        word FGO : 1; // Output ready
    };
    struct{
        word INPR : 8;  // Input register
        word OUTR : 8;  // Output register
    };
} registers, *pRegisters;

void initRegisters(pRegisters regState, word startAddress);

typedef struct processorArgs{
    pRegisters regState;
    word *memory;
    int debugMode;
} processorArgs;

void processor(registers *regState, word *memory, int debugMode);
void *processorThread(void *args);

void ErrorExit(char *str);

void enterRawMode();
void leaveRawMode();

void *teleprinterOutputThread(void *args);
void *teleprinterInputThread(void *args);
/*
    Individual instruction function declarations
*/

#define DEFINE_INSTR(NAME) \
    void NAME(registers *regState)


/*
    Memory instructions
*/

DEFINE_INSTR(and);
DEFINE_INSTR(add);
DEFINE_INSTR(lda);
DEFINE_INSTR(sta);
DEFINE_INSTR(bun);
DEFINE_INSTR(bsa);
DEFINE_INSTR(isz);

/*
    Register instructions
*/

DEFINE_INSTR(cla);
DEFINE_INSTR(cle);
DEFINE_INSTR(cma);
DEFINE_INSTR(cme);
DEFINE_INSTR(cir);
DEFINE_INSTR(cil);
DEFINE_INSTR(inc);
DEFINE_INSTR(spa);
DEFINE_INSTR(sna);
DEFINE_INSTR(sza);
DEFINE_INSTR(sze);
DEFINE_INSTR(hlt);

/*
    Memory instructions
*/

DEFINE_INSTR(inp);
DEFINE_INSTR(out);
DEFINE_INSTR(ski);
DEFINE_INSTR(sko);
DEFINE_INSTR(ion);
DEFINE_INSTR(iof);

#endif