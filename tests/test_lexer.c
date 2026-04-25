//
// Created by zombi on 4/12/2026.
//

// tests/test_lexer.c
// Тесты лексера ToyASM на фреймворке Unity

#include "unity.h"
#include "../inc/lexer.h"

#include <stdio.h>
#include <string.h>



// setUp/tearDown обязательны для Unity, даже если пустые
void setUp(void)    {}
void tearDown(void) {}


void test_should_parse_immediate_values(void)
{
}

void test_should_parse_directives(void)
{
}

void test_should_parse_labels(void)
{
}

void test_should_parse_identifiers(void)
{
}

void test_should_parse_mem_refs(void)
{
}

// ─── точка входа ──────────────────────────────────────────────────────────

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_should_parse_immediate_values);
    RUN_TEST(test_should_parse_directives);


    return UNITY_END();
}