//
// Created by rodrigo on 17/1/21.
//

#include <stdio.h>
#include "compiler.h"
#include "scanner.h"

void compile(VM* vm, const char* source) {
    Scanner scanner;
    init_scanner(&scanner, source);

    int line = -1;
    while (true) {
        Token token = scan_token(&scanner);
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF) break;
    }
}