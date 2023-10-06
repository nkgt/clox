#include <stdio.h>

#include "debug.h"

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int constant_instruction(const char* name, Chunk* chunk, int offset) {
    unsigned char constant_index = chunk->code[offset + 1];
    printf("%-16s %4d '", name, (int)constant_index);
    print_value(chunk->constants.values[constant_index]);
    printf("'\n");
    return offset + 2;
}

void disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);

    for(int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

int disassembleInstruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);

    if(offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }

    unsigned char instruction = chunk->code[offset];
    switch(instruction) {
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", (int)instruction);
            return offset + 1;
    }
}
