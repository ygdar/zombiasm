//
// Created by zombi on 4/11/2026.
//

#pragma once
#include <stdint.h>
#include <stdio.h>

typedef enum {
    TOK_EOF = 0,
    TOK_ERROR,
    TOK_DIRECTIVE,
    TOK_LABEL,
    TOK_INSTRUCTION,
    TOK_IMMEDIATE,
    TOK_BRACKET_OPEN,
    TOK_BRACKET_CLOSE,
    TOK_COMMA,
    TOK_IDENT,
    TOK_NUMBER,
} TokenType_t;

typedef enum {
    LEXER_CREATED = 0,
    LEXER_COMPLETED = 1 << 0,
    LEXER_HAS_ERROR = 1 << 1,
} LexerState_t;

typedef struct {
    TokenType_t type;
    char lexeme[128];
    uint32_t line;
} Token_t;

typedef struct {
    FILE* file;
    uint32_t line;
    char current_token;
    LexerState_t state;
} Lexer_t;


Lexer_t* lexer_ctor(FILE* file);
void lexer_dtor(Lexer_t* lexer);

Token_t lexer_emit_next_token(Lexer_t* lexer);
const char* lexer_format_token_type(TokenType_t type);