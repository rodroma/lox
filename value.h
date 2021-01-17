//
// Created by rodrigo on 17/1/21.
//

#ifndef LOX_VALUE_H
#define LOX_VALUE_H

#include "common.h"

typedef double Value;

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

void init_value_array(ValueArray*);
void write_value_array(ValueArray*, Value);
void free_value_array(ValueArray*);

void print_value(Value);

#endif //LOX_VALUE_H
