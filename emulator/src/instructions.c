#include "emulator.h"

extern word memory[MEMORY_SIZE];

/*
    Memory instructions
*/

DEFINE_INSTR(and){
    switch(regState->SC){
        case 0: // Loading the operand address into MAR
            regState->MAR = GET_OPERAND(regState->MBR);
            break;
        case 1: // Loading the operand itself from the address in MAR
            regState->MBR = memory[regState->MAR];
            break;
        case 2: // Does the bitwise AND itself
            regState->ACC = regState->ACC & regState->MBR;
            break;
        case 3: // Goes to FETCH cycle
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(add){
    switch(regState->SC){
        case 0: // Loading the operand address into MAR
            regState->MAR = GET_OPERAND(regState->MBR);
            break;
        case 1: // Loading the operand itself from the address in MAR
            regState->MBR = memory[regState->MAR];
            break;
        case 2: // Does the addition
            regState->ACC = regState->ACC + regState->MBR;
            break;
        case 3: // Goes to FETCH cycle
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(lda){
    switch(regState->SC){
        case 0: // Loading the operand address into MAR
            regState->MAR = GET_OPERAND(regState->MBR);
            break;
        case 1: // Loading the operand itself from the address in MAR
            regState->MBR = memory[regState->MAR];
            break;
        case 2: // Moves the content of MBR to ACC
            regState->ACC = regState->MBR;
            break;
        case 3: // Goes to FETCH cycle
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(sta){
    switch(regState->SC){
        case 0: // Loading the destination address into MAR
            regState->MAR = GET_OPERAND(regState->MBR);
            break;
        case 1: // Moves the ACC content into MBR
            regState->MBR = regState->ACC;
            break;
        case 2: // Sends the MBR onto address in MAR
            memory[regState->MAR] = regState->MBR;
            break;
        case 3: // Goes to FETCH cycle
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(bun){
    switch(regState->SC){
        case 0: // Loading the destination address into PC
            regState->PC = GET_OPERAND(regState->MBR);
            break;
        case 1:
        case 2:
            break;
        case 3: // Goes to FETCH cycle
            regState->F = 0;
            regState->R = 0;
            break;
    }
}

/*
    BSA jumps to the address provided as its operand and saves the return address on that address
    The first line of the procedure will begin in the word after the word provided as the argument.
    BUN* <address> is to be used for returning to the main program
*/
DEFINE_INSTR(bsa){
   switch(regState->SC){
        case 0: // Loading the destination address into MAR
            regState->MAR = GET_OPERAND(regState->MBR);
            break;
        case 1: // Saving the program counter
            regState->MBR = regState->PC;
            break;
        case 2: // Sending the old PC into memory on address MAR and loading the new one
            regState->PC = regState->MAR;
            memory[regState->MAR] = regState->MBR;
            break;
        case 3: // Incrementing the program counter and going to FETCH cycle
            regState->PC++;
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
/*
    The Increment Skip Zero instruction. I do not know how would this instruction be properly implemented in only 4 tacts so here is this 
    (It increments MBR if that is possible)
*/
DEFINE_INSTR(isz){
    switch(regState->SC){
        case 0: // Loading the operand address into MAR
            regState->MAR = GET_OPERAND(regState->MBR);
            break;
        case 1: // Loading the operand itself from the address in MAR and increments it by one
            regState->MBR = memory[regState->MAR] + 1;
            break;
        case 2: // Sends the incremented operand back into memory
            memory[regState->MAR] = regState->MBR;
            break;
        case 3: // Skipping one instruction if MBR is zero and going to FETCH cycle
            if(regState->MBR == 0) regState->PC++;
            regState->F = 0;
            regState->R = 0;
            break;
    }
}

/*
    Register instructions
*/

// CLA - Clear Accumulator
DEFINE_INSTR(cla){
    switch(regState->SC){
        case 0:
            regState->ACC = 0;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(cle){
    switch(regState->SC){
        case 0:
            regState->E = 0;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(cma){
    switch(regState->SC){
        case 0:
            regState->ACC = ~regState->ACC;
            break;
        case 1:
            regState->ACC++;
            break;
        case 2:
            break; // Empty tact
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(cme){
    switch(regState->SC){
        case 0:
            regState->E = ~regState->E;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(cir){
    static word old = 0; // used merely to save the old E between tacts (because of the function call)
    switch(regState->SC){
        case 0: // Saves the old E
            old = regState->E;
            break;
        case 1: // Sets the new E
            regState->E = regState->ACC & 1;
            break;
        case 2: // Shifts the accumulator right
            regState->ACC >> 1;
            break;
        case 3: // Mends the old E on the MSB, resets the static old
            regState->ACC | (old << 15);
            old = 0;
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(cil){
    static word old = 0; // used merely to save the old E between tacts (because of the function call)
    switch(regState->SC){
        case 0: // Saves the old E
            old = regState->E;
            break;
        case 1: // Sets the new E
            regState->E = (regState->ACC & (1 << 15)) >> 15;
            break;
        case 2: // Shifts the accumulator left
            regState->ACC <<= 1;
            break;
        case 3: // Mends the old E on the MSB, resets the static old
            regState->ACC | (old);
            old = 0;
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(inc){
    switch(regState->SC){
        case 0: // Incrementing the accumulator
            regState->ACC++;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(spa){
    switch(regState->SC){
        case 0: // Skipping one instruction if accumulator is possitive (if MSB is 0)
            if(!(regState->ACC & (1 << 15)) && regState->ACC != 0) regState->PC++;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(sna){
    switch(regState->SC){
        case 0: // Skipping one instruction if accumulator is negative (if MSB is 1)
            if(regState->ACC & (1 << 15)) regState->PC++;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(sza){
    switch(regState->SC){
        case 0: // Skipping one instruction if accumulator is zero
            if(regState->ACC == 0) regState->PC++;
            break;
        case 1:
        case 2:
            break;
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(sze){
    switch(regState->SC){
        case 0: // Skipping one instruction if E is zero
            if(regState->E == 0) regState->PC++;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(hlt){
    switch(regState->SC){
        case 0: // Stopping the computer
            regState->S = 0;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}

/*
    Input-Output Instructions
*/

DEFINE_INSTR(inp){
    switch(regState->SC){
        case 0:
            regState->ACC = regState->INPR;
            regState->FGI = 0;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(out){
    switch(regState->SC){
        case 0:
            regState->OUTR = regState->ACC;
            regState->FGO = 0;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(ski){
    switch(regState->SC){
        case 0:
            if(regState->FGI == 1) regState->PC++;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(sko){
    switch(regState->SC){
        case 0:
            if(regState->FGO == 1) regState->PC++;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(ion){
    switch(regState->SC){
        case 0:
            regState->IEN = 1;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}
DEFINE_INSTR(iof){
    switch(regState->SC){
        case 0:
            regState->IEN = 0;
            break;
        case 1:
        case 2:
            break; // Empty tacts
        case 3:
            regState->F = 0;
            regState->R = 0;
            break;
    }
}