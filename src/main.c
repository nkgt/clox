#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char** argv) {
    init_vm();

    Chunk chunk;
    init_chunk(&chunk);

    unsigned char constant_index = (unsigned char)add_constant(&chunk, 1.2);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant_index, 123);

    write_chunk(&chunk, OP_RETURN, 123);
    disassemble_chunk(&chunk, "test instruction");
    interpret(&chunk);

    free_vm();
    free_chunk(&chunk);
    return 0;
}
