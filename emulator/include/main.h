#ifndef MAIN_H
#define MAIN_H

typedef unsigned short word;
typedef unsigned char byte;

typedef struct lineT{
    short address;
    union{
        unsigned short instr;
        struct{
            unsigned short instr : 4;
            unsigned short opernd : 12;
        } parts;
    };
} lineT;

typedef struct registers{
    
} regs;

#endif