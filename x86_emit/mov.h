#pragma once

#include "code_buffer.h"
#include "util.h"

#include <stdint.h>

EmitStatus_t mov_r64_r64(CodeBuffer_t* codebuffer, Register64_t lhs, Register64_t rhs);

EmitStatus_t mov_r64_mem(CodeBuffer_t* codebuffer, Register64_t lhs, Register64_t rhs_base, int32_t rhs_offset);

EmitStatus_t mov_r32_r32(CodeBuffer_t* codebuffer, Register32_t lhs, Register32_t rhs);

EmitStatus_t mov_r32_mem(CodeBuffer_t* codebuffer, Register32_t lhs, Register64_t rhs_base, int32_t rhs_offset);


