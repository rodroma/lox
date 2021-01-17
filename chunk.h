//
// Created by rodrigo on 17/1/21.
//

#ifndef LOX_CHUNK_H
#define LOX_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_RETURN
} OpCode;

typedef struct {
    int capacity;
    int count;
    uint8_t* code;
    int* lines;
    ValueArray constants;
} Chunk;

void init_chunk(Chunk*);
void write_chunk(Chunk*, uint8_t byte, int line);
int add_constant(Chunk*, Value);
void free_chunk(Chunk*);

#endif //LOX_CHUNK_H
