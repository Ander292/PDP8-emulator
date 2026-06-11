/*
    The built in loaders that load the program executable into memory.
    These aren't executed inside the emulator
*/

#include "emulator.h"

/*
    Literally moves the whole executable into memory absolutely. Band aid solution
    For other types of loaders assembler program will have to be rewriten to actually
    do something
*/
void rawLoader(word *memory, word *rawContent, size_t copySize){
    for(word i = 0; i < copySize; i++){
        memory[i] = rawContent[i];
    }
}