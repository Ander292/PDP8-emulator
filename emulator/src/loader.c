/*
    The built in loaders that load the program executable into memory.
    These aren't executed inside the emulator
*/

#include "emulator.h"

/*
    Literally moves the whole executable into memory absolutely. Band aid solution
    For other types of loaders assembler program will have to be rewriten to actually
    do something.
    TODO: Remove the rawLoader and implement actual loaders
*/
void rawLoader(word *memory, word *rawContent, size_t copySize){
    for(word i = 0; i < copySize; i++){
        memory[i] = rawContent[i];
    }
}

/*
    Exe format for the static loader:
    1) [address] it loads a chunk to
    2) a sequence of words that it reads into memory sequentially
    3) [7777] to stop the chunk and begin a new one. The loader may have as many 
    chunks as needed.

    4) Last chunk that is about to be loaded ends with [FFFF]
    5) Finally the address where the program should start is loaded

    TODO: Write this ASAP
*/

void absoluteLoader(word *memory, word *rawContent, size_t copySize){
    return;
}

/*
    Exe format for the dynamic loader:

    1) [address] it loads a chunk to
    2) [delta] the offset it adds to the operands of all memory instructiosn of the chunk
    - It is determined by the highest 4 bits of the word not being equal to 0xF or 0x7
    3) [7777] to stop the current chunk and begin a new one
    
    4) Last chunk ends with [FFFF] followed by the address at which the program starts
*/

void relativeLoader(word *memory, word *rawContent, size_t copySize){
    return;
}