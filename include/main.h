#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
#include "instrTable.h"

//------------Typedefs------------//

typedef struct lineOg{
    short address;
    char name[4];
    unsigned short operand;
} lineOg;

typedef struct lineT{
    short address;
    unsigned short instr;
} lineT;



#endif