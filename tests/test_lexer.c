//
// Created by zombi on 4/12/2026.
//

// tests/test_lexer.c
// Тесты лексера ToyASM на фреймворке Unity

#include "unity.h"
#include "../inc/lexer.h"

#include <stdio.h>
#include <string.h>

// ─── вспомогательные функции ──────────────────────────────────────────────

// Создаём FILE* из строки в памяти — не нужен файл на диске
static FILE* make_stream(const char* src)
{
#ifdef _WIN32
    // На Windows fmemopen недоступна; пишем во временный файл
    FILE* f = tmpfile();
    fputs(src, f);
    rewind(f);
    return f;
#else
    return fmemopen((void*)src, strlen(src), "r");
#endif
}

// Получить один токен из строки
static Token_t token_from(const char* src)
{
    FILE* f = make_stream(src);
    TokenizerArg_t arg = { .line = 1, .file = f };
    Token_t tok = get_next_token(&arg);
    fclose(f);
    return tok;
}

// Получить N токенов из строки
static void tokens_from(const char* src, Token_t* out, int count)
{
    FILE* f = make_stream(src);
    TokenizerArg_t arg = { .line = 1, .file = f };
    for (int i = 0; i < count; i++)
        out[i] = get_next_token(&arg);
    fclose(f);
}

// setUp/tearDown обязательны для Unity, даже если пустые
void setUp(void)    {}
void tearDown(void) {}


void test_should_parse_immediate_values(void)
{
    const int IMM_VALUES_COUNT = 3;

    static const char* IMM_STR_VALUES[] = {"#0x00", "#0xff", "#1234"};
    static const uint64_t IMM_VALUES[] = {0, 255, 1234};

    for (int ix = 0; ix < IMM_VALUES_COUNT; ix++)
    {
        Token_t token = token_from(IMM_STR_VALUES[ix]);

        TEST_ASSERT_EQUAL(TOK_IMM, token.type);
        TEST_ASSERT_EQUAL_UINT32(1, token.line);
        TEST_ASSERT_EQUAL_UINT64(IMM_VALUES[ix], token.imm_value);
    }
}

void test_should_parse_directives(void)
{
    static const char* DIRECTIVE_STR_VALUES[] = {".org", ".ob12"};
    static const char* DIRECTIVE_VALUES[] = {"org", "ob12"};

    for (int ix = 0; ix < 2; ix++)
    {
        Token_t token = token_from(DIRECTIVE_STR_VALUES[ix]);

        TEST_ASSERT_EQUAL(TOK_DIRECTIVE, token.type);
        TEST_ASSERT_EQUAL_UINT32(1, token.line);
        TEST_ASSERT_EQUAL_STRING(DIRECTIVE_VALUES[ix], token.directive);
    }
}

// ─── точка входа ──────────────────────────────────────────────────────────

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_should_parse_immediate_values);
    RUN_TEST(test_should_parse_directives);

    // // Регистры
    // RUN_TEST(test_reg_r0);
    // RUN_TEST(test_reg_r7);
    // RUN_TEST(test_reg_lowercase);
    //
    // // Immediate
    // RUN_TEST(test_imm_zero);
    // RUN_TEST(test_imm_simple);
    // RUN_TEST(test_imm_max_byte);
    // RUN_TEST(test_imm_large);
    //
    // // Метки
    // RUN_TEST(test_label_definition);
    // RUN_TEST(test_label_reference);
    // RUN_TEST(test_label_underscore);
    //
    // // Разделители
    // RUN_TEST(test_comma);
    // RUN_TEST(test_newline);
    // RUN_TEST(test_eof);
    //
    // // Счётчик строк
    // RUN_TEST(test_line_counter_increments);

    // // Пробелы и комментарии
    // RUN_TEST(test_skip_leading_spaces);
    // RUN_TEST(test_skip_tabs);
    // RUN_TEST(test_comment_skipped);
    //
    // // Полные инструкции
    // RUN_TEST(test_full_instruction_ldi);
    // RUN_TEST(test_full_instruction_add);
    // RUN_TEST(test_full_instruction_jmp_label);
    // RUN_TEST(test_label_then_instruction);
    // RUN_TEST(test_hello_asm_sequence);

    return UNITY_END();
}