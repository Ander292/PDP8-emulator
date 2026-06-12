#ifndef SHARED_H
#define SHARED_H

#include <stdint.h> // For typedefs

#define MEMORY_SIZE 4096
#define WORD_SIZE sizeof(word)

#define IS_LETTER(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

typedef uint16_t word;
typedef uint8_t byte;

#endif