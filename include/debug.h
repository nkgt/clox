#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION

#include "chunk.h"

void disassemble_chunk(Chunk* chunk, const char* name);
int disassemble_instruction(Chunk* chunk, int offset);

#endif //CLOX_DEBUG_H
