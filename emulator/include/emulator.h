#ifndef EMULATOR_H
#define EMULATOR_H

#define MEMORY_SIZE 4096
#define WORD_SIZE 2

#define MEMORY_INSTRUCTION 0
#define REGISTER_INSTRUCTION 1
#define IO_INSTRUCTION 2

#define GET_INSTRUCTION(memWord) (((instr)((word)memWord)).parts.instr)
#define GET_OPERAND(memWord) (((instr)((word)memWord)).parts.opernd)
#define GET_TYPE(memWord) (GET_INSTRUCTION(memWord) != 0x7 ? MEMORY_INSTRUCTION : (GET_INSTRUCTION(memWord) & 0x8 ? IO_INSTRUCTION : REGISTER_INSTRUCTION))

#define GET_CYCLE(f, r) ((f) << 1 + (r))
#define CYCLE_FETCH 0
#define CYCLE_INDIRECT 1
#define CYCLE_EXECUTE 2
#define CYCLE_INTERRUPT 3

typedef unsigned short word;
typedef unsigned char byte;

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
    };
} registers, *pRegisters;

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

#endif