#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
} ValueType;

typedef struct {
    ValueType type;

    union {
        bool boolean;
        double number;
    } as;
} Value;

typedef struct {
    size_t capacity;
    size_t count;
    Value* values;
} ValueArray;

void init_value_array(ValueArray* array);
void write_value_array(ValueArray* array, Value value);
void free_value_array(ValueArray* array);
void print_value(Value value);
bool is_number(Value value);
bool values_equal(Value a, Value b);

#endif //CLOX_VALUE_H
