#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "scanner.h"

#define BYTES(...) ((uint8_t[]){__VA_ARGS__}), sizeof((uint8_t[]){__VA_ARGS__}) / sizeof(uint8_t)

typedef struct {
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNEMENT,
    PREC_OR,
    PREC_AND,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY,
} Precedence;

typedef void (*ParseFn)(void);

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

Parser parser;
Chunk* compiling_chunk;

static void expression(void);
static void grouping(void);
static void binary(void);
static void number(void);
static void unary(void);
static void literal(void);

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
    [TOKEN_BANG]          = {unary,    NULL,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_GREATER]       = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_LESS]          = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL]    = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
    [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FALSE]         = {literal,  NULL,   PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NIL]           = {literal,  NULL,   PREC_NONE},
    [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_TRUE]          = {literal,  NULL,   PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static Chunk* current_chunk(void) {
    return compiling_chunk;
}

static void error_at(Token* token, const char* message) {
    if(parser.panic_mode) return;
    parser.panic_mode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if(token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if(TOKEN_ERROR) {
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.had_error = true;

}

static void error_at_current(const char* message) {
    error_at(&parser.previous, message);
}

static void advance(void) {
    parser.previous = parser.current;
    
    for(;;) {
        parser.current = scan_token();
        if(parser.current.type != TOKEN_ERROR) break;

        error_at_current(parser.current.start);
    }
}

static void consume(TokenType type, const char* message) {
    if(parser.current.type == type) {
        advance();
        return;
    }

    error_at_current(message);
}

static inline void emit_bytes(uint8_t *bytes, int size) {
    for(int i = 0; i < size; ++i) {
        write_chunk(current_chunk(), bytes[i], parser.previous.line);
    }
}

static void parse_precedence(Precedence precedence) {
    advance();

    ParseFn prefix_rule = rules[parser.previous.type].prefix;
    if(prefix_rule == NULL) {
        error_at(&parser.previous, "Expected expression.");
        return;
    }

    prefix_rule();

    while(precedence <= rules[parser.current.type].precedence) {
        advance();
        ParseFn infix_rule = rules[parser.previous.type].infix;
        infix_rule();
    }
}

static void expression(void) {
    parse_precedence(PREC_ASSIGNEMENT);
}

static void grouping(void) {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expected ')' after expression.");
}

static void binary(void) {
    TokenType operator_type = parser.previous.type;
    ParseRule rule = rules[operator_type]; 
    parse_precedence((Precedence)(rule.precedence + 1));

    switch (operator_type) {
        case TOKEN_BANG_EQUAL:    emit_bytes(BYTES(OP_EQUAL, OP_NOT));   break;
        case TOKEN_EQUAL_EQUAL:   emit_bytes(BYTES(OP_EQUAL));           break;
        case TOKEN_GREATER:       emit_bytes(BYTES(OP_GREATER));         break;
        case TOKEN_GREATER_EQUAL: emit_bytes(BYTES(OP_LESS, OP_NOT));    break;
        case TOKEN_LESS:          emit_bytes(BYTES(OP_LESS));            break;
        case TOKEN_LESS_EQUAL:    emit_bytes(BYTES(OP_GREATER, OP_NOT)); break;
        case TOKEN_PLUS:          emit_bytes(BYTES(OP_ADD));             break;
        case TOKEN_MINUS:         emit_bytes(BYTES(OP_SUBTRACT));        break;
        case TOKEN_STAR:          emit_bytes(BYTES(OP_MULTIPLY));        break;
        case TOKEN_SLASH:         emit_bytes(BYTES(OP_DIVIDE));          break;
        default: return;
    }
}

static void literal(void) {
    switch (parser.previous.type) {
        case TOKEN_FALSE: emit_bytes(BYTES(OP_FALSE)); return;
        case TOKEN_NIL:   emit_bytes(BYTES(OP_NIL));   return;
        case TOKEN_TRUE:  emit_bytes(BYTES(OP_TRUE));  return;
        default: return;
    }
}

static uint8_t make_constant(Value value) {
    int constant = (int)add_constant(current_chunk(), value);

    if(constant > UINT8_MAX) {
        error_at(&parser.previous, "Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void number(void) {
    double num_value = strtod(parser.previous.start, NULL);
    Value value = {VAL_NUMBER, {.number = num_value}};

    emit_bytes(BYTES(OP_CONSTANT));
    emit_bytes(BYTES(make_constant(value))); 
}

static void unary(void) {
    TokenType operator_type = parser.previous.type;

    parse_precedence(PREC_UNARY);

    switch(operator_type) {
        case TOKEN_BANG:  emit_bytes(BYTES(OP_NOT));    break;
        case TOKEN_MINUS: emit_bytes(BYTES(OP_NEGATE)); break;
        default: return;
    }
}

bool compile(const char* source, Chunk* chunk) {
    init_scanner(source);

    compiling_chunk = chunk;

    parser.had_error = false;
    parser.panic_mode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expected end of expression.");

    emit_bytes(BYTES(OP_RETURN));

#ifdef DEBUG_PRINT_CODE
    if(!parser.had_error) {
        disassemble_chunk(current_chunk(), "code");
    }
#endif

    return !parser.had_error;
}
