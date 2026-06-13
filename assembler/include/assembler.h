#ifndef MAIN_H
#define MAIN_H

#include "shared.h"



//------------Typedefs------------//

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