#ifndef CLOX_COMPILER_H 
#define CLOX_COMPILER_H

#include <stdbool.h>

#include "chunk.h"

bool compile(const char* source, Chunk* chunk);

#endif // CLOX_COMPILER_H
