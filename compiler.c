//
// Created by rodrigo on 17/1/21.
//

#include <stdio.h>
#include "compiler.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

// Forward declarations

static void advance(Parser*);
static void consume(Parser*, TokenType, const char* message);

static void error_at(Parser*, Token*, const char* message);
static void error(Parser*, const char* message);
static void error_at_current(Parser*, const char* message);

static void emit_byte(Parser*, uint8_t);
static void emit_bytes(Parser*, uint8_t, uint8_t);
static void emit_constant(Parser*, Value);

static void end_compiler(Parser*);

static void unary(Parser*);
static void expression(Parser*);
static void grouping(Parser*);
static void number(Parser*);
static void binary(Parser*);

static void parse_precedence(Parser*, Precedence);

typedef void (*ParseFn)(Parser*);

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence  precedence
} ParseRule;

static ParseRule* get_rule(TokenType);
ParseRule rules[] = {
    [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
    [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
    [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
    [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
    [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
    [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

// Public

bool compile(const char* source, Chunk* chunk) {
    Scanner scanner;
    init_scanner(&scanner, source);

    Parser parser = {
        .scanner = &scanner,
        .panic_mode = false,
        .had_error = false,

        .compiling_chunk = chunk,
    };

    advance(&parser);
    expression(&parser);

    consume(&parser, TOKEN_EOF, "Expect end of expression.");
    end_compiler(&parser);

    return !parser.had_error;
}

// Private

static void binary(Parser* parser) {
    // Remember the operator.
    TokenType operator_type = parser->previous.type;

    // Compile the right operand.
    ParseRule* rule = get_rule(operator_type);
    parse_precedence(parser, (Precedence) (rule->precedence + 1));

    switch (operator_type) {
        case TOKEN_PLUS:  emit_byte(parser, OP_ADD); break;
        case TOKEN_MINUS: emit_byte(parser, OP_SUBTRACT); break;
        case TOKEN_STAR:  emit_byte(parser, OP_MULTIPLY); break;
        case TOKEN_SLASH: emit_byte(parser, OP_DIVIDE); break;
        default: return; // Unreachable
    }
}

static void expression(Parser* parser) {
    parse_precedence(parser, PREC_ASSIGNMENT);
}

static void unary(Parser* parser) {
    TokenType operator_type = parser->previous.type;

    // Compile the operand.
    parse_precedence(parser, PREC_UNARY);

    // Emit the operator instruction.
    switch (operator_type) {
        case TOKEN_MINUS: {
            emit_byte(parser, OP_NEGATE);
            break;
        }
        default:
            return; // Unrecheable
    }
}

static void number(Parser* parser) {
    double value = strtod(parser->previous.start, NULL);
    emit_constant(parser, value);
}

static void grouping(Parser* parser) {
    expression(parser);
    consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void advance(Parser* parser) {
    parser->previous = parser->current;

    while (true) {
        parser->current = scan_token(parser->scanner);
        if (parser->current.type != TOKEN_ERROR) break;

        error_at_current(parser, parser->current.start);
    }
}

static void error_at_current(Parser* parser, const char* message) {
    error_at(parser, &parser->current, message);
}

static void error(Parser* parser, const char* message) {
    error_at(parser, &parser->previous, message);
}

static void error_at(Parser* parser, Token* token, const char* message) {
    if (parser->panic_mode) return;
    parser->panic_mode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing.
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser->had_error = true;
}

static void consume(Parser* parser, TokenType type, const char* message) {
    if (parser->current.type == type) {
        advance(parser);
        return;
    }

    error_at_current(parser, message);
}

static Chunk* current_chunk(Parser* parser) {
    return parser->compiling_chunk;
}

static ParseRule* get_rule(TokenType type) {
    return &rules[type];
}

static void parse_precedence(Parser* parser, Precedence precedence) {
    advance(parser);
    ParseFn prefix_rule = get_rule(parser->previous.type)->prefix;
    if (!prefix_rule) {
        error(parser, "Expect expression.");
        return;
    }

    prefix_rule(parser);

    while(precedence <= get_rule(parser->current.type)->precedence) {
        advance(parser);
        ParseFn infix_rule = get_rule(parser->previous.type)->infix;
        infix_rule(parser);
    }
}

// Emits

static void emit_byte(Parser* parser, uint8_t byte) {
    write_chunk(current_chunk(parser), byte, parser->previous.line);
}

static void emit_bytes(Parser* parser, uint8_t byte1, uint8_t byte2) {
    emit_byte(parser, byte1);
    emit_byte(parser, byte2);
}

static void emit_return(Parser* parser) {
    emit_byte(parser, OP_RETURN);
}

static uint8_t make_constant(Parser* parser, Value value) {
    int constant = add_constant(current_chunk(parser), value);
    if (constant > UINT8_MAX) {
        error(parser, "Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t) constant;
}

static void emit_constant(Parser* parser, Value value) {
    emit_bytes(parser, OP_CONSTANT, make_constant(parser, value));
}

static void end_compiler(Parser* parser) {
    emit_return(parser);
#ifdef DEBUG_PRINT_CODE
    if (!parser->had_error) {
        dissasemble_chunk(current_chunk(parser), "code");
    }
#endif
}