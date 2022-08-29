#pragma once

#include <x86_emit/code_buffer.h>
#include <x86_emit/util.h>
#include <stdint.h>

EmitStatus_t movss_xmm_mem(CodeBuffer_t* codebuffer, XMM_t lhs, Register64_t rhs_base, int32_t rhs_offset);

EmitStatus_t movsd_xmm_mem(CodeBuffer_t* codebuffer, XMM_t lhs, Register64_t rhs_base, int32_t rhs_offset);


