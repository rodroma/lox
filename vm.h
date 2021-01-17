//
// Created by rodrigo on 17/1/21.
//

#ifndef LOX_VM_H
#define LOX_VM_H

#include "chunk.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stack_top;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpreterResult;

void init_vm(VM*);
void free_vm(VM*);

InterpreterResult interpret(VM* vm, const char* source);
void push(VM*, Value);
Value pop(VM*);

#endif //LOX_VM_H
