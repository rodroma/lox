#include <stdio.h>

#include "chunk.h"
#include "debug.h"

int main() {
    Chunk chunk;

    init_chunk(&chunk);

    int constant = add_constant(&chunk, 1.2);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant, 123);

    write_chunk(&chunk, OP_RETURN, 123);

    dissasemble_chunk(&chunk, "test chunk");

    free_chunk(&chunk);

    printf("Hello, World!\n");
    return 0;
}
