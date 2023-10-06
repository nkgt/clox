#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include <stdint.h>

#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_RETURN,
} OpCode;

typedef struct {
    int capacity;
    int count;
    uint8_t* code;
    int* lines;
    ValueArray constants;
} Chunk;

void init_chunk(Chunk* chunk);
void free_chunk(Chunk* chunk);
void write_chunk(Chunk* chunk, uint8_t byte, int line);
int add_constant(Chunk* chunk, Value value);

#endif //CLOX_CHUNK_H
