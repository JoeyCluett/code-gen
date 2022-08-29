#pragma once

#include <array>
#include <stdint.h>

typedef unsigned char byte_t;

enum EmitStatus_t {
    SUCCESS = 0,
    FAILURE = 1
};

enum class REX : byte_t {
    Base = 0x40,
    W    = 0x08, // promotes 32-bit value to 64-bit
    R    = 0x04,
    X    = 0x02,
    B    = 0x01  // exposes 8 upper GP registers
};

// generic size reference
enum SizeReference_t : byte_t {
    _8b  = 0,
    _16b = 1,
    _32b = 2,
    _64b = 3
};

SizeReference_t offset_size_i32(int32_t i32);

typedef std::array<byte_t, 4> i32_buffer_t;
i32_buffer_t i32_to_buffer(int32_t i32);

enum Register8_t : byte_t {
    AL = 0, CL = 1, DL = 2, BL = 3,
    AH = 4, CH = 5, DH = 6, BH = 7
};

enum Register16_t : byte_t {
    AX = 0,    CX = 1,    DX = 2,    BX = 3,
    SP = 4,    BP = 5,    SI = 6,    DI = 7,
    R8w = 8,   R9w = 9,   R10w = 10, R11w = 11,
    R12w = 12, R13w = 13, R14w = 14, R15w = 15
};

enum Register32_t : byte_t {
    EAX  =  0, ECX  =  1, EDX  =  2, EBX  =  3,
    ESP  =  4, EBP  =  5, ESI  =  6, EDI  =  7,
    R8d  =  8, R9d  =  9, R10d = 10, R11d = 11, 
    R12d = 12, R13d = 13, R14d = 14, R15d = 15
};

enum Register64_t : byte_t {
    RAX =  0, RCX =  1, RDX =  2, RBX =  3, 
    RSP =  4, RBP =  5, RSI =  6, RDI =  7,
    R8  =  8, R9  =  9, R10 = 10, R11 = 11, 
    R12 = 12, R13 = 13, R14 = 14, R15 = 15
};

enum XMM_t : byte_t {
    XMM0  = 0,  XMM1  = 1,  XMM2  = 2,  XMM3  = 3,
    XMM4  = 4,  XMM5  = 5,  XMM6  = 6,  XMM7  = 7,
    XMM8  = 8,  XMM9  = 9,  XMM10 = 10, XMM11 = 11,
    XMM12 = 12, XMM13 = 13, XMM14 = 14, XMM15 = 15,
};
