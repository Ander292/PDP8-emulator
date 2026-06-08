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
        int shouldPrint = 0;

        pthread_mutex_lock(&outputMutex);
        if(regState->FGO == 0) shouldPrint = 1;
        pthread_mutex_unlock(&outputMutex);

        if(shouldPrint){
            pthread_mutex_lock(&outputMutex);
            byte c = regState->OUTR;
            regState->FGO = 1;
            pthread_mutex_unlock(&outputMutex);
            putchar(c);
            fflush(stdout);
        }

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
        int shouldInput = 0;
        pthread_mutex_lock(&inputMutex);
        if(regState->FGI == 0) shouldInput = 1;
        pthread_mutex_unlock(&inputMutex);
        if(shouldInput) {
            byte c = pollInput(POLL_TIMEOUT);
            //byte c = getchar();
            if(c != 0) {
                pthread_mutex_lock(&inputMutex);
                regState->INPR = c;
                regState->FGI = 1; // The program is now ready to read input
                printf("FGI was set to 1 by input");
                pthread_mutex_unlock(&inputMutex);
                printf("\nteleIn reports %d:%c\n", c, c);
            }
        }
        //sleepF(POLL_TIMEOUT);
    }
}



void *teleprinterInputThread(void *args){
    //printf("Input thread started...\n");
    teleprinterIn((pRegisters)args);
    //printf("Input thread is about to end...\n");
    return NULL;
}