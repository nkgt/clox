#include <stdarg.h>
#include <stdio.h>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "vm.h"

typedef Value (*binary_op)(double, double);

VM vm;

static void inline reset_stack(void) {
    vm.stack_top = vm.stack;
}

static void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    long instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    reset_stack();
}

static inline Value is_falsey(Value value) {
    bool result =  value.type == VAL_NIL ||
                  (value.type == VAL_BOOL && !value.as.boolean);
    return (Value){VAL_BOOL, {.boolean = result}};
}

static inline bool binary(binary_op op) {
    if(!is_number(vm.stack_top[-1]) || !is_number(vm.stack_top[-2])) {
        runtime_error("Operands must be numbers");
        return false;
    }

    --vm.stack_top;
    vm.stack_top[-1] = op(vm.stack_top[-1].as.number, vm.stack_top[0].as.number); 
    return true;
}

static inline Value sum(double a, double b) {
    return (Value){VAL_NUMBER, {.number = a + b}};
}

static inline Value sub(double a, double b) {
    return (Value){VAL_NUMBER, {.number = a - b}};
}

static inline Value mul(double a, double b) {
    return (Value){VAL_NUMBER, {.number = a * b}};
}

static inline Value div(double a, double b) {
    return (Value){VAL_NUMBER, {.number = a / b}};
}

static inline Value greater(double a, double b) {
    return (Value){VAL_BOOL, {.boolean = a > b}};
}

static inline Value less(double a, double b) {
    return (Value){VAL_BOOL, {.boolean = a < b}};
}

static interpret_result run(void) {
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
            case OP_NIL:   push((Value){VAL_NIL,  {.number = 0}});      break;
            case OP_TRUE:  push((Value){VAL_BOOL, {.boolean = true}});  break;
            case OP_FALSE: push((Value){VAL_BOOL, {.boolean = false}}); break;
            case OP_EQUAL:
                --vm.stack_top;
                bool result = values_equal(vm.stack_top[-1], vm.stack_top[0]);
                vm.stack_top[-1] = (Value){VAL_BOOL, {.boolean = result}};
                break;
            case OP_GREATER:
                if(!binary(greater)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            case OP_LESS:
                if(!binary(less)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            case OP_ADD:
                if(!binary(sum)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            case OP_SUBTRACT:
                if(!binary(sub)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            case OP_MULTIPLY:
                if(!binary(mul)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            case OP_DIVIDE:
                if(!binary(div)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            case OP_NOT:
                vm.stack_top[-1] = is_falsey(vm.stack_top[-1]);
                break;
            case OP_NEGATE:
                if(vm.stack_top[-1].type != VAL_NUMBER) {
                    runtime_error("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                vm.stack_top[-1].as.number *= -1; 
                break;
        }
    }
}

void init_vm(void) {
    reset_stack();
}

void free_vm(void) {}

interpret_result interpret(const char* source) {
    Chunk chunk;
    init_chunk(&chunk);

    if(!compile(source, &chunk)) {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    interpret_result result = run();

    free_chunk(&chunk);
    return result;
}

void push(Value value) {
    *vm.stack_top = value;
    ++vm.stack_top;
}

Value pop(void) {
    --vm.stack_top;
    return *vm.stack_top;
}
