//
// Created by rodrigo on 17/1/21.
//

#include <stdio.h>

#include "memory.h"

void* reallocate(void* pointer, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void* result = realloc(pointer, new_size);

    if (!result) {
        fprintf(stderr, "could not realloc memory, failing...");
        exit(1);
    }

    return result;
}