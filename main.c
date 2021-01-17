#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "vm.h"

#define EXIT_BAD_ARGUMENT_COUNT 64
#define EXIT_COMPILE_ERROR 65
#define EXIT_RUNTIME_ERROR 70
#define EXIT_COULD_NOT_READ_FILE 74

// Forward declarations
static void repl(VM*);
static void run_file(VM*, const char* path);

// Main

int main(int argc, const char* argv[]) {
    VM vm;
    init_vm(&vm);

    if (argc == 1) {
        repl(&vm);
    } else if (argc == 2) {
        run_file(&vm, argv[1]);
    } else {
        fprintf(stderr, "Usage: lox [path]\n");
        exit(EXIT_BAD_ARGUMENT_COUNT);
    }

    free_vm(&vm);

    return EXIT_SUCCESS;
}

static void repl(VM* vm) {
    char line[1024];
    while (true) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        interpret(vm, line);
    }
}

static char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");

    if (!file) {
        fprintf(stderr, "Could not open file '%s'.", path);
        exit(EXIT_COULD_NOT_READ_FILE);
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);

    if (!buffer) {
        fprintf(stderr, "Not enough memory to read '%s'.", path);
        exit(EXIT_COULD_NOT_READ_FILE);
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);

    if (bytes_read < file_size) {
        fprintf(stderr, "Could not read file '%s'.", path);
        exit(EXIT_COULD_NOT_READ_FILE);
    }

    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

static void run_file(VM* vm, const char* path) {
    char* source = read_file(path);
    InterpreterResult result = interpret(vm, source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) exit(EXIT_COMPILE_ERROR);
    if (result == INTERPRET_RUNTIME_ERROR) exit(EXIT_RUNTIME_ERROR);
}