#ifndef INSTR_TABLE_H
#define INSTR_TABLE_H
#include "instr.h"
/*
    The translation table that is used when an instruction string 
    is needed to be translated into the relevant instruction code.
*/

typedef struct translation_info{
    char name[5];
    unsigned short number;
} TranslInfo;

TranslInfo MemoryInstrTable[] = { // Only 4 bits, the rest is operand
    {"AND", AND}, {"AND*", IAND},
    {"ADD", ADD}, {"ADD*", IADD},
    {"LDA", LDA}, {"LDA*", ILDA},
    {"STA", STA}, {"STA*", ISTA},
    {"BUN", BUN}, {"BUN*", IBUN},
    {"BSA", BSA}, {"BSA*", IBSA},
    {"ISZ", ISZ}, {"ISZ*", IISZ}
};

TranslInfo RegisterInstrTable[] = { // 4 bytes, has no operand
    {"CLA", CLA},
    {"CLE", CLE},
    {"CMA", CMA},
    {"CME", CME},
    {"CIR", CIR},
    {"CIL", CIL},
    {"INC", INC},
    {"SPA", SPA},
    {"SNA", SNA},
    {"SZA", SZA},
    {"SZE", SZE},
    {"HLT", HLT}
};

TranslInfo IoInstrTable[] = { // 4 bytes, has no operand
    {"INP", INP},
    {"OUT", OUT},
    {"SKI", SKI},
    {"SKO", SKO},
    {"ION", ION},
    {"IOF", IOF}
};

#endif //INSTR_TABLE_H