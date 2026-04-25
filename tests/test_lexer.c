#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

/* ──────────────────────────────────────────────────────────────
 * Вспомогательные утилиты
 * ────────────────────────────────────────────────────────────── */

/** Создает временный файл из строки и сбрасывает позицию в начало */
static FILE* create_test_stream(const char* content) {
    FILE* f = tmpfile();
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "tmpfile() failed. Check system limits.");
    if (content) {
        fputs(content, f);
        rewind(f);
    }
    return f;
}

/** Безопасная проверка токена с диагностикой */
static void expect_token(Lexer_t* lex, TokenType_t type, const char* expected_lexeme, uint32_t expected_line) {
    Token_t tok = lexer_emit_next_token(lex);
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(type, tok.type, "Token type mismatch");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_lexeme, tok.lexeme, "Token lexeme mismatch");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected_line, tok.line, "Line number mismatch");
}

void setUp(void) { /* Инициализация перед каждым тестом */ }
void tearDown(void) { /* Очистка после каждого теста */ }

/* ──────────────────────────────────────────────────────────────
 * Группа 1: Базовые токены и форматы чисел
 * ────────────────────────────────────────────────────────────── */
void test_numbers_and_directives(void) {
    FILE* f = create_test_stream(".org 0x0000\n.data 0b1010 123\n");
    Lexer_t* lex = lexer_ctor(f);
    TEST_ASSERT_NOT_NULL(lex);

    // Примечание: в текущей реализации accept_directive не добавляет '.' в lexeme
    expect_token(lex, TOK_DIRECTIVE, "org", 1);
    expect_token(lex, TOK_NUMBER, "0x0000", 1);

    expect_token(lex, TOK_DIRECTIVE, "data", 2);
    expect_token(lex, TOK_NUMBER, "0b1010", 2);
    expect_token(lex, TOK_NUMBER, "123", 2);
    expect_token(lex, TOK_EOF, "<EOF>", 3);

    lexer_dtor(lex);
    fclose(f);
}

void test_immediate_values(void) {
    FILE* f = create_test_stream("#255 #0xFF #0b1\n");
    Lexer_t* lex = lexer_ctor(f);

    expect_token(lex, TOK_IMMEDIATE, "255", 1);
    expect_token(lex, TOK_IMMEDIATE, "0xff", 1); // Текущий код приводит hex к нижнему регистру
    expect_token(lex, TOK_IMMEDIATE, "0b1", 1);
    expect_token(lex, TOK_EOF, "<EOF>", 2);

    lexer_dtor(lex);
    fclose(f);
}

/* ──────────────────────────────────────────────────────────────
 * Группа 2: Метки, идентификаторы и скобки
 * ────────────────────────────────────────────────────────────── */
void test_labels_and_identifiers(void) {
    FILE* f = create_test_stream("start:\nJMP loop\ndone:\n");
    Lexer_t* lex = lexer_ctor(f);

    expect_token(lex, TOK_LABEL, "start", 1);  // ':' поглощается, в lexeme не попадает
    // Примечание: TOK_INSTRUCTION/TOK_REGISTER пока не реализованы в lexer.c,
    // поэтому они возвращаются как TOK_IDENT. Это ожидаемо для текущей версии.
    expect_token(lex, TOK_IDENT, "JMP", 2);
    expect_token(lex, TOK_IDENT, "loop", 2);
    expect_token(lex, TOK_LABEL, "done", 3);
    expect_token(lex, TOK_EOF, "<EOF>", 4);

    lexer_dtor(lex);
    fclose(f);
}

void test_memory_operands_and_punctuation(void) {
    FILE* f = create_test_stream("LD R3, [R5]\nPOP R1, [0x10]\n");
    Lexer_t* lex = lexer_ctor(f);

    expect_token(lex, TOK_IDENT, "LD", 1);
    expect_token(lex, TOK_IDENT, "R3", 1);
    expect_token(lex, TOK_COMMA, ",", 1);
    expect_token(lex, TOK_BRACKET_OPEN, "[", 1);
    expect_token(lex, TOK_IDENT, "R5", 1);
    expect_token(lex, TOK_BRACKET_CLOSE, "]", 1);

    expect_token(lex, TOK_IDENT, "POP", 2);
    expect_token(lex, TOK_IDENT, "R1", 2);
    expect_token(lex, TOK_COMMA, ",", 2);
    expect_token(lex, TOK_BRACKET_OPEN, "[", 2);
    expect_token(lex, TOK_NUMBER, "0x10", 2);
    expect_token(lex, TOK_BRACKET_CLOSE, "]", 2);

    expect_token(lex, TOK_EOF, "<EOF>", 3);

    lexer_dtor(lex);
    fclose(f);
}

/* ──────────────────────────────────────────────────────────────
 * Группа 3: Комментарии, пробелы и переносы строк
 * ────────────────────────────────────────────────────────────── */
void test_comments_and_whitespace_skipping(void) {
    FILE* f = create_test_stream("  NOP  ; this is ignored\n   HLT\n");
    Lexer_t* lex = lexer_ctor(f);

    expect_token(lex, TOK_IDENT, "NOP", 1);
    expect_token(lex, TOK_IDENT, "HLT", 2); // Комментарий и пробелы пропущены
    expect_token(lex, TOK_EOF, "<EOF>", 3);

    lexer_dtor(lex);
    fclose(f);
}

void test_line_tracking_accuracy(void) {
    FILE* f = create_test_stream("A\n\nB ; comment\nC\n");
    Lexer_t* lex = lexer_ctor(f);

    expect_token(lex, TOK_IDENT, "A", 1);
    expect_token(lex, TOK_IDENT, "B", 3); // Два переноса + комментарий
    expect_token(lex, TOK_IDENT, "C", 4);
    expect_token(lex, TOK_EOF, "<EOF>", 5);

    lexer_dtor(lex);
    fclose(f);
}

/* ──────────────────────────────────────────────────────────────
 * Группа 4: Обработка ошибок и состояние лексера
 * ────────────────────────────────────────────────────────────── */
void test_error_detection_and_halt(void) {
    FILE* f = create_test_stream("ADD R1, @ BAD\n");
    Lexer_t* lex = lexer_ctor(f);

    expect_token(lex, TOK_IDENT, "ADD", 1);
    expect_token(lex, TOK_IDENT, "R1", 1);
    expect_token(lex, TOK_COMMA, ",", 1);

    Token_t err = lexer_emit_next_token(lex);
    TEST_ASSERT_EQUAL(TOK_ERROR, err.type);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("<UNKNOWN>", err.lexeme, "Token lexeme mismatch");

    // Проверка флага ошибки в состоянии
    TEST_ASSERT_BITS(LEXER_HAS_ERROR, LEXER_HAS_ERROR, lex->state);

    // Лексер должен продолжать возвращать ERROR или остановиться
    Token_t after_err = lexer_emit_next_token(lex);
    TEST_ASSERT_EQUAL(TOK_ERROR, after_err.type);

    lexer_dtor(lex);
    fclose(f);
}

void test_invalid_number_syntax(void) {
    FILE* f = create_test_stream("# 123\n0xG\n");
    Lexer_t* lex = lexer_ctor(f);

    // "#" без цифры сразу за ним
    Token_t err1 = lexer_emit_next_token(lex);
    TEST_ASSERT_EQUAL(TOK_ERROR, err1.type);
    TEST_ASSERT_BITS(LEXER_HAS_ERROR, LEXER_HAS_ERROR, lex->state);

    lexer_dtor(lex);
    fclose(f);
}

/* ──────────────────────────────────────────────────────────────
 * Группа 5: Защита от переполнения буфера
 * ────────────────────────────────────────────────────────────── */
void test_buffer_overflow_protection(void) {
    char long_ident[200];
    memset(long_ident, 'A', sizeof(long_ident) - 1);
    long_ident[sizeof(long_ident) - 1] = '\0';

    FILE* f = create_test_stream(long_ident);
    Lexer_t* lex = lexer_ctor(f);

    Token_t tok = lexer_emit_next_token(lex);
    TEST_ASSERT_EQUAL(TOK_IDENT, tok.type);
    TEST_ASSERT_EQUAL(127, strlen(tok.lexeme)); // Буфер 128 + '\0'
    TEST_ASSERT_EQUAL('A', tok.lexeme[0]);

    lexer_dtor(lex);
    fclose(f);
}

/* ──────────────────────────────────────────────────────────────
 * Main
 * ────────────────────────────────────────────────────────────── */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_numbers_and_directives);
    RUN_TEST(test_immediate_values);
    RUN_TEST(test_labels_and_identifiers);
    RUN_TEST(test_memory_operands_and_punctuation);
    RUN_TEST(test_comments_and_whitespace_skipping);
    RUN_TEST(test_line_tracking_accuracy);
    RUN_TEST(test_error_detection_and_halt);
    RUN_TEST(test_invalid_number_syntax);
    RUN_TEST(test_buffer_overflow_protection);
    return UNITY_END();
}