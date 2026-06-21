#include "emulator.h"
#include "system.h"
#include <stdio.h>
#include <pthread.h>
/*
    The input-output device that this architecture has access to.
    stdout is simulated as teleprinter output
    stdin is simulated as the input
    Input is done character by character
    Mutexes are used to prevent race condition and character loss
*/

#define OUTPUT_LATENCY 1 // How long in ms takes to print one character
//#define INPUT_LATENCY 1 // How long in ms takes to input one char
#define POLL_TIMEOUT 1 // How long in ms should the polling cooldown be (to not burn the processor)

/*
    The teleprinter will print the character in OUTR and set FGO to 1
    This function will sleep after writting to
    simulate slow hardware.
*/

void teleprinterOut(pRegisters regState){
    //puts("Before out!");
    while(regState->S | regState->FGO == 0){

        pthread_mutex_lock(&outputMutex);
        if(regState->FGO == 0){
            putchar(regState->OUTR);
            regState->FGO = 1;
            fflush(stdout);
        }
        pthread_mutex_unlock(&outputMutex);
        sleepF(POLL_TIMEOUT); 
    }
    //puts("After out!");
}
void *teleprinterOutputThread(void *args){
    teleprinterOut((pRegisters)args);
    //printf("Output is gone...\n");
    //putchar('\n');
    return NULL;
}

/*
    Input functions. Teleprinter will send an interrupt (set FGI register to 1)
    when a character is inputed
*/

void teleprinterIn(pRegisters regState){
    //puts("Before in!");
    while(regState->S){
        pthread_mutex_lock(&inputMutex);
        if(regState->FGI == 0) {
            //puts("Called poll");
            byte c = pollInput(POLL_TIMEOUT);
            //puts("Out of poll");
            if(c != 0) {
                regState->INPR = c;
                regState->FGI = 1; // The program is now ready to read input
            }
        }
        pthread_mutex_unlock(&inputMutex);
        //sleepF(POLL_TIMEOUT);
    }
    //puts("After in!");
}



void *teleprinterInputThread(void *args){
    teleprinterIn((pRegisters)args);
    //printf("Input is gone...\n");
    return NULL;
}