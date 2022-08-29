#include "movfp.h"

#include <x86_emit/code_buffer.h>
#include <x86_emit/util.h>
#include <stdint.h>

//
// special cases for when offset is zero
//
static EmitStatus_t mov_xmm_mem_off0(CodeBuffer_t* codebuffer, byte_t prefix, XMM_t lhs, Register64_t rhs_base);

//
// encoding for non-zero offsets
// handles 8- and 32-bit offsets
//
static EmitStatus_t mov_xmm_mem_offnz(CodeBuffer_t* codebuffer, byte_t prefix, XMM_t lhs, Register64_t rhs_base, int32_t rhs_offset);

//
// special case for non-zero offset with RSP
//
static EmitStatus_t mov_xmm_mem_offnz_rsp(CodeBuffer_t* codebuffer, byte_t opcode, XMM_t lhs, int32_t rhs_offset);

EmitStatus_t movss_xmm_mem(CodeBuffer_t* codebuffer, XMM_t lhs, Register64_t rhs_base, int32_t rhs_offset) {

    byte_t prefix = 0xf3;

    if(rhs_offset == 0) { // optimized routine when offset is 0
        return mov_xmm_mem_off0(codebuffer, prefix, lhs, rhs_base);
    } else {
        return mov_xmm_mem_offnz(codebuffer, prefix, lhs, rhs_base, rhs_offset);
    }
}

EmitStatus_t movsd_xmm_mem(CodeBuffer_t* codebuffer, XMM_t lhs, Register64_t rhs_base, int32_t rhs_offset) {

    byte_t prefix = 0xf2;

    if(rhs_offset == 0) { // optimized routine when offset is 0
        return mov_xmm_mem_off0(codebuffer, prefix, lhs, rhs_base);
    } else {
        return mov_xmm_mem_offnz(codebuffer, prefix, lhs, rhs_base, rhs_offset);
    }
}

static EmitStatus_t mov_xmm_mem_off0(CodeBuffer_t* codebuffer, byte_t prefix, XMM_t lhs, Register64_t rhs_base) {

    if(lhs >= XMM_t::XMM8 || rhs_base >= Register64_t::R8) {
        
        byte_t buf[] = { // technically, the full opcode is [ prefix, 0x0f, 0x10 ] but the REX byte has to go after other prefixes
            prefix,            // mandatory prefix for SSE
            (byte_t)REX::Base, // REX byte
            0x0f, 0x10, // opcode
            0x00  // operands
        };

        buf[4] |= (byte_t)((lhs << 3) & 0b00111000);
        buf[4] |= (byte_t)(rhs_base & 0b0111);

        if(lhs >= XMM_t::XMM8)
            buf[1] |= (byte_t)REX::R;

        if(rhs_base >= Register64_t::R8)
            buf[1] |= (byte_t)REX::B;

        CodeBuffer_emit_raw(codebuffer, buf, buf + 5);
        return EmitStatus_t::SUCCESS;

    } else if(rhs_base == Register64_t::RSP) { // RSP is special because x86 is dumb

        byte_t operand_byte = 0x04 | (byte_t)((lhs << 3) & 0b00111000);

        byte_t buf[] = {
            0x00, prefix, 0x0f, 0x10, operand_byte, 0x24 };

        if(lhs >= XMM_t::XMM8) {
            buf[0] = prefix;
            buf[1] = 0x44; // <-- properly formatted REX
            CodeBuffer_emit_raw(codebuffer, buf, buf + 6);
        } else {
            CodeBuffer_emit_raw(codebuffer, buf + 1, buf + 6);
        }

        return EmitStatus_t::SUCCESS;

    } else { // all registers are lower

        byte_t buf[] = {
            prefix, 0x0f, 0x10,
            0x00 // <-- operand byte
        };

        buf[3] |= (byte_t)((lhs << 3) & 0b00111000);
        buf[3] |= (byte_t)(rhs_base & 0b0111);

        CodeBuffer_emit_raw(codebuffer, buf, buf + 4);
        return EmitStatus_t::SUCCESS;
    }
}

static EmitStatus_t mov_xmm_mem_offnz(CodeBuffer_t* codebuffer, byte_t prefix, XMM_t lhs, Register64_t rhs_base, int32_t rhs_offset) {

    if(rhs_base == Register64_t::RSP)
        return mov_xmm_mem_offnz_rsp(codebuffer, prefix, lhs, rhs_offset);

    union {
        int32_t i32;
        byte_t buf[4];
    } u;

    u.i32 = rhs_offset;

    byte_t buf[] = {
        0x00, // <-- could become 0xf3
        prefix, // <-- could becode REX flag
        0x0f, 0x10,
        0x40, // <-- becomes 0x80 if offset is 32-bit
        u.buf[0], u.buf[1], u.buf[2], u.buf[3] // <-- might be 8- or 32-bit offset
    };

    byte_t* const offset_ptr   = buf + 5;
    byte_t* const operand_byte = buf + 4;

    byte_t* instruction_start = buf + 1; // changes if upper registers are used
    byte_t* REX_flag_ptr      = NULL; // nothing yet
    byte_t* end_iter          = buf + 6; // assumes starting with 8-bit

    if(lhs >= XMM_t::XMM8 || rhs_base >= Register64_t::R8) {
        // upper regisers are used
        buf[0] = prefix;
        buf[1] = (byte_t)REX::Base;
        instruction_start = buf;
        REX_flag_ptr = buf + 1;
    }

    if(rhs_offset < -128 || rhs_offset > 127) {
        // offset is definitely 32 bits now
        *operand_byte = 0x80;
        end_iter = buf + 9;
    }

    *operand_byte |= (byte_t)((lhs << 3) & 0b00111000);
    *operand_byte |= (byte_t)(rhs_base & 0b0111);

    if(lhs >= XMM_t::XMM8)
        *REX_flag_ptr |= (byte_t)REX::R;
    
    if(rhs_base >= Register64_t::R8)
        *REX_flag_ptr |= (byte_t)REX::B;

    CodeBuffer_emit_raw(codebuffer, instruction_start, end_iter);
    return EmitStatus_t::SUCCESS;
}

static EmitStatus_t mov_xmm_mem_offnz_rsp(CodeBuffer_t* codebuffer, byte_t prefix, XMM_t lhs, int32_t rhs_offset) {

    union {
        int32_t i32;
        byte_t buf[4];
    } u;

    byte_t buf[] = {
        0x00, // becomes 0xf3 if REX byte is needed
        prefix, // becomes REX byte if upper XMM register is used
        0x0f, 0x10,
        0x44, // <-- becomes 0x84 if offset is 32 bits
        0x24,
        u.buf[0], u.buf[1], u.buf[2], u.buf[3]
    };

    byte_t* start_iter  = buf + 1;
    byte_t* end_iter    = buf + 7; // assumes 8-bit offset at first
    byte_t* REX_ptr     = NULL;
    byte_t* operand_ptr = buf + 4;

    if(lhs >= XMM_t::XMM8) {
        buf[0] = prefix;
        buf[1] = (byte_t)REX::Base;
        REX_ptr = buf + 1;
        start_iter = buf;
    }

    // promote offset to 32 bits
    if(rhs_offset < -128 || rhs_offset > 127)
        *operand_ptr = 0x84;

    *operand_ptr |= (byte_t)((lhs << 3) & 0b00111000);
    if(lhs >= XMM_t::XMM8)
        *REX_ptr |= (byte_t)REX::R;

    CodeBuffer_emit_raw(codebuffer, start_iter, end_iter);
    return EmitStatus_t::SUCCESS;
}

