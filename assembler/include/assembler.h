#ifndef MAIN_H
#define MAIN_H

#include "shared.h"

//------------Typedefs------------//
#if 0
typedef struct lineOg{
    short address;
    char name[5];
    unsigned short operand;
} lineOg;

typedef struct lineT{
    short address;
    union{
        unsigned short instr;
        struct{
            unsigned short opernd : 12;
            unsigned short instr : 4;
        } parts;
    };
} lineT;
#else
typedef struct lineT{
    word type;
    word address;

    word opCode;
    word operand;
} lineT;

typedef struct lineOg{
    word address;
    char name[5];
    word operand;
} lineOg;

#endif

#endif