#include <stdio.h>

#include "memory.h"
#include "value.h"

void init_value_array(ValueArray* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void write_value_array(ValueArray* array, Value value) {
    if(array->capacity < array->count + 1) {
        size_t old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values = GROW_ARRAY(
            Value,
            array->values,
            old_capacity,
            array->capacity
        );
    }

    array->values[array->count] = value;
    ++array->count;
}

void free_value_array(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    init_value_array(array);
}

void print_value(Value value) {
    switch (value.type) {
        case VAL_BOOL:
            printf(value.as.boolean ? "true" : "false");
            break;
        case VAL_NIL: printf("nil"); break;
        case VAL_NUMBER: printf("%g", value.as.number); break;
    }
}

bool is_number(Value value) {
    return value.type == VAL_NUMBER;
}

bool values_equal(Value a, Value b) {
    if(a.type != b.type) {
        return false; 
    }

    switch (a.type) {
        case VAL_BOOL:   return a.as.boolean == b.as.boolean; 
        case VAL_NIL:    return true;
        case VAL_NUMBER: return a.as.number == b.as.number;
    }
}
