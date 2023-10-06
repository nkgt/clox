#include <stdio.h>

#include "debug.h"
#include "vm.h"

VM vm;

static void reset_stack(void) {
    vm.stack_top = vm.stack;
}

static interpret_result run(void) {
#define BINARY_OP(op) \
    { \
        --vm.stack_top; \
        *(vm.stack_top - 1) = *(vm.stack_top - 1) op *vm.stack_top; \
    } 

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");

        for(Value* slot = vm.stack; slot < vm.stack_top; ++slot) {
            printf("[ ");
            print_value(*slot);
            printf(" ]");
        }

        printf("\n");
        disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instruction = *vm.ip++;

        switch (instruction) {
            case OP_RETURN: {
                print_value(pop());
                printf("\n");
                return INTERPRET_OK;
            }
            case OP_CONSTANT: {
                Value constant = vm.chunk->constants.values[*vm.ip++];
                push(constant);
                break;
            }
            case OP_ADD:      BINARY_OP(+) break;
            case OP_SUBTRACT: BINARY_OP(-) break;
            case OP_MULTIPLY: BINARY_OP(*) break;
            case OP_DIVIDE:   BINARY_OP(/) break;
            case OP_NEGATE: *(vm.stack_top - 1) = -*(vm.stack_top - 1); break;
        }
    }

#undef BINARY_OP
}

void init_vm(void) {
    reset_stack();
}

void free_vm(void) {}

interpret_result interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;

    return run();
}

void push(Value value) {
    *vm.stack_top = value;
    ++vm.stack_top;
}

Value pop(void) {
    --vm.stack_top;
    return *vm.stack_top;
}
