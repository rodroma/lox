//
// Created by rodrigo on 17/1/21.
//

#include <stdio.h>
#include "vm.h"
#include "debug.h"
#include "compiler.h"

// Forward declarations

static InterpreterResult run(VM*);
static void reset_stack(VM*);

// Public

void init_vm(VM* vm) {
    reset_stack(vm);
}

void free_vm(VM* vm) {

}

InterpreterResult interpret(VM* vm, const char* source) {
    Chunk chunk;
    init_chunk(&chunk);

    if (!compile(source, &chunk)) {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm->chunk = &chunk;
    vm->ip = chunk.code;

    InterpreterResult result = run(vm);

    free_chunk(&chunk);

    return result;
}

void push(VM* vm, Value value) {
    *vm->stack_top = value;
    vm->stack_top += 1;
}

Value pop(VM* vm) {
    vm->stack_top -= 1;
    return *vm->stack_top;
}

// Private

static InterpreterResult run(VM* vm) {
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)      \
    do {                   \
        Value b = pop(vm); \
        Value a = pop(vm); \
        push(vm, a op b);  \
    } while(0)

    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm->stack; slot < vm->stack_top; slot += 1) {
            printf("[ ");
            print_value(*slot);
            printf(" ]");
        }
        printf("\n");
        dissasemble_instruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
#endif

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(vm, constant);
                break;
            }
            case OP_NEGATE: {
                push(vm, -pop(vm));
                break;
            }
            case OP_ADD: BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE: BINARY_OP(/); break;
            case OP_RETURN: {
                print_value(pop(vm));
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }
#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

static void reset_stack(VM* vm) {
    vm->stack_top = vm->stack;
}