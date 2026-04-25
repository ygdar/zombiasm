#include <stdio.h>
#include <stdlib.h>

#include "inc/lexer.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        perror("[ERROR] You need to specify file");
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("[ERROR] Failed to open file");
        return EXIT_FAILURE;
    }

    Lexer_t* lex = lexer_ctor(fp);
    if (!lex) {
        fprintf(stderr, "[ERROR] Failed to initialize lexer\n");
        fclose(fp);
        return EXIT_FAILURE;
    }

    int token_count = 0;
    int error_count = 0;

    printf("%-5s | %-15s | %s\n", "Line", "Type", "Lexeme");
    printf("%-5s | %-15s | %s\n", "", "", "");

    while (1) {
        Token_t token = lexer_emit_next_token(lex);
        printf("%-5d | %-15s | %s\n", token.line, lexer_format_token_type(token.type), token.lexeme);
        token_count++;

        if (token.type == TOK_EOF) {
            break;
        }
        if (token.type == TOK_ERROR) {
            error_count++;
            // Лексер внутренне останавливает работу после первой ошибки
            break;
        }
    }

    lexer_dtor(lex);
    fclose(fp);

    printf("\n[SUMMARY] Processed %d token(s).", token_count);
    if (error_count > 0) {
        fprintf(stderr, "\n[ERROR] Lexing aborted due to %d error(s).\n", error_count);
        return EXIT_FAILURE;
    } else {
        printf(" Success.\n");
        return EXIT_SUCCESS;
    }

}