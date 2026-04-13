//
// Created by zombi on 4/11/2026.
//

#include "inc/lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
    Register_t reg;
    char name[3];
} RegisterName_t;

typedef struct
{
    Opcode_t opcode;
    char name[5];
} OpcodeName_t;

static uint8_t advance(FILE* file)
{
    return (uint8_t)fgetc(file);
}

static int peek(FILE* file)
{
    const int ch = fgetc(file);
    ungetc(ch, file);
    return ch;
}

bool try_get_imm_and_advance(FILE* file, uint64_t* imm_value)
{
    if (peek(file) != '#')
    {
        return false;
    }

    advance(file);

    uint8_t buffer[64] = { '\0' };
    for (int ix = 0; isalnum(peek(file)); ix++)
    {
        buffer[ix] = advance(file);
    }

    // Проверить errno
    *imm_value = strtoul(buffer, NULL, 0);

    return true;
}

bool try_get_directive_and_advance(FILE* file, uint8_t* directive)
{
    if (peek(file) != '.')
    {
        return false;
    }

    advance(file);

    for (int ix = 0; isalnum(peek(file)); ix++)
    {
        directive[ix] = advance(file);
    }

    return true;
}

bool try_get_label_and_advance(FILE* file, uint8_t* label)
{
    if (!isalpha(peek(file)))
    {
        return false;
    }

    for (int ix = 0; isalnum(peek(file)); ix++)
    {
        label[ix] = advance(file);
    }

    if (peek(file) == ':')
    {
        advance(file);
        return true;
    }

    fseek(file, -1 * (int) strlen(label), SEEK_CUR);
    return false;
}

bool try_get_identifier_and_advance(FILE* file, uint8_t* identifier)
{
    if (!isalpha(peek(file)))
    {
        return false;
    }

    for (int ix = 0; isalnum(peek(file)); ix++)
    {
        identifier[ix] = advance(file);
    }

    return true;
}

bool try_get_mem_ref_and_advance(FILE* file, uint8_t* mem_ref)
{
    if (peek(file) != '[')
    {
        return false;
    }

    advance(file);

    if (!try_get_identifier_and_advance(file, mem_ref))
    {
        return false;
    }

    if (peek(file) != ']')
    {
        return false;
    }

    advance(file);
    return true;
}


Token_t get_next_token(TokenizerArg_t* arg)
{
    // Пропускаем пробелы
    while (peek(arg->file) == ' ' || peek(arg->file) == '\t')
    {
        advance(arg->file);
    }

    // Пропускаем комментарии
    if (peek(arg->file) == ';')
    {
        while (peek(arg->file) != '\n')
        {
            advance(arg->file);
        }
    }

    if (peek(arg->file) == ',')
    {
        advance(arg->file);

        return (Token_t) {
            .type = TOK_COMMA,
            .line = arg->line,
        };
    }

    if (peek(arg->file) == '\n')
    {
        advance(arg->file);

        return (Token_t) {
            .type = TOK_NEWLINE,
            .line = arg->line,
        };
    }

    if (peek(arg->file) == EOF)
    {
        advance(arg->file);

        return (Token_t) {
            .type = TOK_EOF,
            .line = arg->line,
        };
    }

    uint64_t imm_value = 0;
    if (try_get_imm_and_advance(arg->file, &imm_value))
    {
        return (Token_t) {
            .type = TOK_IMM,
            .line = arg->line,
            .imm_value = imm_value,
        };
    }

    uint8_t directive[64] = { '\0' };
    if (try_get_directive_and_advance(arg->file, directive))
    {
        Token_t token = {
            .type = TOK_DIRECTIVE,
            .line = arg->line,
        };

        strcpy(token.directive, directive);
        return token;
    }

    uint8_t label[64] = { '\0' };
    if (try_get_label_and_advance(arg->file, label))
    {
        Token_t token = {
            .type = TOK_LABEL,
            .line = arg->line,
        };

        strcpy(token.label, label);
        return token;
    }

    uint8_t identifier[64] = { '\0' };
    if (try_get_identifier_and_advance(arg->file, identifier))
    {
        Token_t token = {
            .type = TOK_IDENTIFIER,
            .line = arg->line,
        };

        strcpy(token.identifier, identifier);
        return token;
    }

    uint8_t mem_ref[64] = { '\0' };
    if (try_get_mem_ref_and_advance(arg->file, mem_ref))
    {
        Token_t token = {
            .type = TOK_MEM_REF,
            .line = arg->line,
        };

        strcpy(token.identifier, identifier);
        return token;
    }

    fprintf(stderr, "Invalid char %c at line %d", peek(arg->file), arg->line);
    exit(1);
}