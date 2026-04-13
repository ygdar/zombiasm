//
// Created by zombi on 4/11/2026.
//

#pragma once
#include <stdint.h>
#include <stdio.h>

#include "common.h"


typedef enum
{
    TOK_REG,
    TOK_DIRECTIVE,
    TOK_LABEL,
    TOK_IDENTIFIER,
    TOK_MEM_REF,
    TOK_COMMA,
    TOK_IMM,
    TOK_NEWLINE,
    TOK_EOF,
} TokenType_t;

typedef struct
{
    uint32_t line;
    FILE* file;
} TokenizerArg_t;

typedef struct
{
    TokenType_t type;
    uint32_t line;
    union
    {
        Register_t reg;
        uint64_t imm_value;
        uint8_t label[64];
        uint8_t directive[64];
        uint8_t identifier[64];
    };
} Token_t;


Token_t get_next_token(TokenizerArg_t* arg);