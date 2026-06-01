#ifndef INSTR_TABLE_H
#define INSTR_TABLE_H

/*
    Defines for all instructions that are currently in use.
*/

typedef struct translation_info{
    char name[5];
    unsigned short number;
} TranslInfo;

TranslInfo MemoryInstrTable[] = { // Only 1 byte, the rest is operand
    {"AND", 0x0U}, {"AND*", 0x8U},
    {"ADD", 0x1U}, {"ADD*", 0x9U},
    {"LDA", 0x2U}, {"LDA*", 0xAU},
    {"STA", 0x3U}, {"STA*", 0xBU},
    {"BUN", 0x4U}, {"BUN*", 0xCU},
    {"BSA", 0x5U}, {"BSA*", 0xDU},
    {"ISZ", 0x6U}, {"ISZ*", 0xEU}
};

TranslInfo RegisterInstrTable[] = { // 4 bytes, has no operand
    {"CLA", 0x7800U},
    {"CLE", 0x7400U},
    {"CMA", 0x7200U},
    {"CME", 0x7100U},
    {"CIR", 0x7080U},
    {"CIL", 0x7040U},
    {"INC", 0x7020U},
    {"SPA", 0x7010U},
    {"SNA", 0x7008U},
    {"SZA", 0x7004U},
    {"SZE", 0x7002U},
    {"HLT", 0x7001U}
};

TranslInfo IoInstrTable[] = { // 4 bytes, has no operand
    {"INP", 0xF800U},
    {"OUT", 0xF400U},
    {"SKI", 0xF200U},
    {"SKO", 0xF100U},
    {"ION", 0xF080U},
    {"IOF", 0xF040U}
};

#endif //INSTR_TABLE_H