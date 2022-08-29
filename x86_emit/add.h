#pragma once

#include "code_buffer.h"
#include "util.h"

#include <stdint.h>

// reg += reg
// reg += mem[reg + off]
// reg += constant
// mem[reg]       += reg
// mem[reg + off] += constant

EmitStatus_t add_r32_i32(CodeBuffer_t* codebuffer, Register32_t lhs, int32_t i32);

//EmitStatus_t add_r64_i32(CodeBuffer_t* codebuffer, Register32_t lhs, int32_t i32);

EmitStatus_t add_r32_mem(CodeBuffer_t* codebuffer, Register32_t lhs, Register64_t rhs_base, int32_t rhs_off);
EmitStatus_t add_r64_mem(CodeBuffer_t* codebuffer, Register64_t lhs, Register64_t rhs_base, int32_t rhs_off);

EmitStatus_t add_r8_r8(CodeBuffer_t* codebuffer, Register8_t lhs,  Register8_t rhs);
EmitStatus_t add_r16_r16(CodeBuffer_t* codebuffer, Register16_t lhs, Register16_t rhs);
EmitStatus_t add_r32_r32(CodeBuffer_t* codebuffer, Register32_t lhs, Register32_t rhs);
EmitStatus_t add_r64_r64(CodeBuffer_t* codebuffer, Register64_t lhs, Register64_t rhs);


