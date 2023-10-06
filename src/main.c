#include "chunk.h"
#include "debug.h"

int main() {
    Chunk chunk;
    init_chunk(&chunk);

    unsigned char constant_index = (unsigned char)add_constant(&chunk, 1.2);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant_index, 123);

    write_chunk(&chunk, OP_RETURN, 123);
    disassembleChunk(&chunk, "test instruction");
    free_chunk(&chunk);
    return 0;
}
