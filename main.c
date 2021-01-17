#include <stdio.h>

#include "chunk.h"
#include "vm.h"

int main() {
    VM vm;
    init_vm(&vm);

    Chunk chunk;

    init_chunk(&chunk);

    int constant = add_constant(&chunk, 1.2);

    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant, 123);

    constant = add_constant(&chunk, 3.4);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant, 123);

    constant = add_constant(&chunk, 5.6);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant, 123);

    write_chunk(&chunk, OP_DIVIDE, 123);
    write_chunk(&chunk, OP_NEGATE, 123);

    write_chunk(&chunk, OP_RETURN, 123);

    interpret(&vm, &chunk);

    free_vm(&vm);
    free_chunk(&chunk);

    return 0;
}
