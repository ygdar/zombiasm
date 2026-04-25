#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static char peek(const Lexer_t* lexer)
{
    char ch = (char)fgetc(lexer->file);
    ungetc(ch, lexer->file);

    return ch;
}

static char advance(const Lexer_t* lexer)
{
    return (char) fgetc(lexer->file);
}

static void skip_whitespaces(Lexer_t* lexer)
{
    for (;;)
    {
        if (isspace(peek(lexer)) && peek(lexer) != EOF)
        {
            if (advance(lexer) == '\n')
            {
                lexer->line++;
            };
            continue;
        }

        if (peek(lexer) == ';')
        {
            while (peek(lexer) != '\n' && peek(lexer) != EOF)
            {
                advance(lexer);
            }

            continue;
        }

        break;
    }
}

static bool accept_identifier(Lexer_t* lexer, char* lexeme, size_t max_length)
{
    char start_symbol = peek(lexer);

    int ix = 0;
    while (isalnum(peek(lexer)) && ix < max_length - 1)
    {
        lexeme[ix++] = advance(lexer);
    }

    lexeme[ix] = '\0';

    return isalpha(start_symbol) && ix > 0;
}

static bool accept_directive(Lexer_t* lexer, char* lexeme, size_t max_length)
{
    // accept .
    char start_symbol = advance(lexer);

    bool valid = accept_identifier(lexer, lexeme, max_length);

    return start_symbol == '.' && valid;
}

static bool accept_numerical_value(Lexer_t* lexer, char* lexeme, size_t max_length)
{
    char start_symbol = advance(lexer);
    char number_system_symbol = (char)tolower(peek(lexer));

    int ix = 0;

    lexeme[ix++] = start_symbol;
    if (start_symbol == '0' && number_system_symbol == 'b')
    {
        lexeme[ix++] = advance(lexer);
        while ((peek(lexer) == '0' || peek(lexer) == '1') && ix < max_length - 3)
        {
            lexeme[ix++] = advance(lexer);
        }
    }
    else if (start_symbol == '0' && number_system_symbol == 'x')
    {
        lexeme[ix++] = advance(lexer);
        while (isxdigit(peek(lexer)) && ix < max_length - 3)
        {
            lexeme[ix++] = tolower(advance(lexer));
        }
    }
    else
    {
        while (isdigit(peek(lexer)) && ix < max_length - 2)
        {
            lexeme[ix++] = advance(lexer);
        }
    }

    lexeme[ix] = '\0';

    if (number_system_symbol == 'b' || number_system_symbol == 'x')
    {
        return ix > 1;
    }

    return ix > 0;
}

static bool accept_immediate_value(Lexer_t* lexer, char* lexeme, size_t max_length)
{
    char start_symbol = advance(lexer);

    bool accepted = accept_numerical_value(lexer, lexeme, max_length);
    return start_symbol == '#' && accepted;
}

Lexer_t * lexer_ctor(FILE *file)
{
    if (!file) return NULL;

    Lexer_t* lexer = calloc(1, sizeof(Lexer_t));
    if (!lexer) return NULL;

    lexer->file = file;
    lexer->line = 1;
    lexer->state = LEXER_IN_PROCESS;

    return lexer;
}

void lexer_dtor(Lexer_t *lexer)
{
    free(lexer);
}

Token_t lexer_emit_next_token(Lexer_t *lexer)
{
    skip_whitespaces(lexer);

    Token_t token = {
        .line = lexer->line,
    };

    if (peek(lexer) == EOF)
    {
        token.type = TOK_EOF;
        strcpy(token.lexeme, "<EOF>");

        return token;
    }

    if (peek(lexer) == ',')
    {
        advance(lexer);
        token.type = TOK_COMMA; strcpy(token.lexeme, ",");

        return token;
    }

    if (peek(lexer) == '[')
    {
        advance(lexer);
        token.type = TOK_BRACKET_OPEN; strcpy(token.lexeme, "[");

        return token;
    }

    if (peek(lexer) == ']')
    {
        advance(lexer);
        token.type = TOK_BRACKET_CLOSE; strcpy(token.lexeme, "]");

        return token;
    }

    if (peek(lexer) == '.')
    {
        bool valid = accept_directive(lexer, token.lexeme, sizeof(token.lexeme));
        token.type = valid ? TOK_DIRECTIVE : TOK_ERROR;

        return token;
    }

    if (peek(lexer) == '#')
    {
        bool valid = accept_immediate_value(lexer, token.lexeme, sizeof(token.lexeme));
        token.type = valid ? TOK_IMMEDIATE : TOK_ERROR;

        return token;
    }

    if (isdigit(peek(lexer)))
    {
        bool valid = accept_numerical_value(lexer, token.lexeme, sizeof(token.lexeme));
        token.type = valid ? TOK_NUMBER : TOK_ERROR;

        return token;
    }

    if (isalpha(peek(lexer)))
    {
        accept_identifier(lexer, token.lexeme, sizeof(token.lexeme));

        if (peek(lexer) == ':')
        {
            token.type = TOK_LABEL;
            advance(lexer);
        }
        else
        {
            token.type = TOK_IDENT;
        }

        return token;
    }

    return token;
}

const char* lexer_format_token_type(TokenType_t type)
{
    switch (type) {
        case TOK_EOF: return "EOF";
        case TOK_ERROR: return "ERROR";
        case TOK_DIRECTIVE: return "DIRECTIVE";
        case TOK_LABEL: return "LABEL";
        case TOK_INSTRUCTION: return "INSTRUCTION";
        case TOK_IMMEDIATE: return "IMMEDIATE";
        case TOK_BRACKET_OPEN: return "BRACKET_OPEN";
        case TOK_BRACKET_CLOSE: return "BRACKET_CLOSE";
        case TOK_COMMA: return "COMMA";
        case TOK_IDENT: return "IDENT";
        case TOK_NUMBER: return "NUMBER";
        default: return "UNKNOWN";
    }
}


