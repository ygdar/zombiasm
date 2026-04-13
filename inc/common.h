
//
// Created by zombi on 4/11/2026.
//

#pragma once

typedef enum {
    OP_NOP  = 0x00,
    OP_MOV  = 0x01,
    OP_LDI  = 0x02,
    OP_LD   = 0x03,
    OP_ST   = 0x04,
    OP_ADD  = 0x05,
    OP_SUB  = 0x06,
    OP_AND  = 0x07,
    OP_OR   = 0x08,
    OP_XOR  = 0x09,
    OP_SHL  = 0x0A,
    OP_SHR  = 0x0B,
    OP_CMP  = 0x0C,
    OP_JMP  = 0x0D,
    OP_JZ   = 0x0E,
    OP_JN   = 0x0F,
    OP_JC   = 0x10,
    OP_CALL = 0x11,
    OP_RET  = 0x12,
    OP_PUSH = 0x13,
    OP_POP  = 0x14,
    OP_INT  = 0x15,
    OP_IRET = 0x16,
    OP_HALT  = 0x17,
    OP_COUNT
} Opcode_t;

typedef enum
{
    REG_0 = 0x00,
    REG_1 = 0x01,
    REG_2 = 0x02,
    REG_3 = 0x03,
    REG_4 = 0x04,
    REG_5 = 0x05,
    REG_6 = 0x06,
    REG_7 = 0x07,
    REG_COUNT
} Register_t;

typedef enum
{
    FLAG_ZERO = 0x00,
    FLAG_NEG = 0x01,
    FLAG_CARRY = 0x02,
} Flag_t;