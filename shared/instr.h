/*
    This header contains the codes for every implemented instruction 
    in the processor. Everything in the program should use these macros instead
    of manually typing in the instruction codes.
*/

/* 
    Memory instructions
    Format: [I][Code][Operand]
    BitSize: 1    3      12
*/

    #define AND 0x0U
    #define ADD 0x1U
    #define LDA 0x2U
    #define STA 0x3U
    #define BUN 0x4U
    #define BSA 0x5U
    #define ISZ 0x6U

    // The indirect instruction codes written manually if ever needed

    #define IAND 0x8U
    #define IADD 0x9U
    #define ILDA 0xAU
    #define ISTA 0xBU
    #define IBUN 0xCU
    #define IBSA 0xDU
    #define IISZ 0xEU

/* 
    Register instructions 
    The first 4 bits are always equal to 7. (IOPR == 0111) 
    This is the way how the processor indentifies that its not a memory instruction
    Format: [0][111][Instruction Code]
    BitSize: 1   3          12
*/

    #define CLA 0x7800U
    #define CLE 0x7400U
    #define CMA 0x7200U
    #define CME 0x7100U
    #define CIR 0x7080U
    #define CIL 0x7040U
    #define INC 0x7020U
    #define SPA 0x7010U
    #define SNA 0x7008U
    #define SZA 0x7004U
    #define SZE 0x7002U
    #define HLT 0x7001U

/* 
    Input-Output instructions 
    The first 4 bits are always equal to 15. (IOPR == 1111)
    Format: [1][111][Instruction Code]
    BitSize: 1   3          12
*/

    #define INP 0xF800U
    #define OUT 0xF400U
    #define SKI 0xF200U
    #define SKO 0xF100U
    #define ION 0xF080U
    #define IOF 0xF040U