#include <stdio.h>

#include "debug.h"
#include "vm.h"

VM vm;

static interpret_result run(void) {
    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instruction = *vm.ip++;

        switch (instruction) {
            case OP_RETURN: return INTERPRET_OK;
            case OP_CONSTANT: {
                Value constant = vm.chunk->constants.values[*vm.ip++];
                print_value(constant);
                printf("\n");
                break;
            }
        }
    }
}

void init_vm(void) {}
void free_vm(void) {}

interpret_result interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;

    return run();
}
