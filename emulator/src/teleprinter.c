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

#define OUTPUT_LATENCY 5 // How long in ms takes to print one character
#define POLL_TIMEOUT 50 // How long in ms should the polling cooldown be (to not burn the processor)

/*
    The teleprinter will print the character in OUTR and set FGO to 1
    This function will be called async and will sleep before writting to
    simulate slow hardware
*/

void teleprinterOut(pRegisters regState){
    while(regState->S){
        pthread_mutex_lock(&outputMutex);
        if(regState->FGO == 0){
            putchar(regState->OUTR);
            regState->FGO = 1;
            fflush(stdout);
        }
        pthread_mutex_unlock(&outputMutex);

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

/*
    Input functions. Teleprinter will send an interrupt (set FGI register to 1)
    when a character is inputed
*/

void teleprinterIn(pRegisters regState){
    while(regState->S){
        pthread_mutex_lock(&inputMutex);
        if(regState->FGI == 0) {
            byte c = pollInput(POLL_TIMEOUT);
            if(c != 0) {
                regState->INPR = c;
                regState->FGI = 1; // The program is now ready to read input
                // printf("FGI was set to 1 by input");
                // printf("\nteleIn reports %d:%c\n", c, c);
            }
            pthread_mutex_unlock(&inputMutex);
        }
        sleepF(POLL_TIMEOUT);
    }
}



void *teleprinterInputThread(void *args){
    //printf("Input thread started...\n");
    teleprinterIn((pRegisters)args);
    //printf("Input thread is about to end...\n");
    return NULL;
}