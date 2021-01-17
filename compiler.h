//
// Created by rodrigo on 17/1/21.
//

#include "vm.h"
#include "scanner.h"

#ifndef LOX_COMPILER_H
#define LOX_COMPILER_H

typedef struct {
    Token current;
    Token previous;
    Scanner* scanner;
    bool had_error;
    bool panic_mode;

    Chunk* compiling_chunk;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} Precedence;

bool compile(const char* source, Chunk*);

#endif //LOX_COMPILER_H
