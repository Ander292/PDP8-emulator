#include "emulator.h"
#include "system.h"
#include <stdio.h>
#include <pthread.h>
/*
    The input-output device that this architecture has access to.
    stdout is simulated as teleprinter output
    stdin is simulated as the input
    Input is done character by character
*/

#define OUTPUT_LATENCY 50
#define POLL_TIMEOUT 100
/*
    The teleprinter will print the character in OUTR and set FGO to 1
    This function will be called async and will sleep before writting to
    simulate slow hardware
*/
void printCharacter(pRegisters regState){
    sleepF(OUTPUT_LATENCY);
    putchar(regState->OUTR); // Prints the character
    regState->FGO = 1; // Sets the flag back to 1
}


void teleprinterOut(pRegisters regState){
    while(regState->S || (regState->FGO == 0)){
        sleepF(POLL_TIMEOUT);
        while(regState->FGO == 0){
            printCharacter(regState);
        }
    }
}

void teleprinterIn(pRegisters regState){
    while(regState->S){
        sleepF(POLL_TIMEOUT);
    }
}

void *teleprinterOutputThread(void *args){
    //printf("Out thread started...\n");
    teleprinterOut((pRegisters)args);
    putchar('\n');
    //printf("Output thread is about to end...\n");
    return NULL;
}


void *teleprinterInputThread(void *args){
    //printf("Input thread started...\n");
    teleprinterIn((pRegisters)args);
    //printf("Input thread is about to end...\n");
    return NULL;
}