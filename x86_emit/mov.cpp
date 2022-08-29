#include "mov.h"

#include "code_buffer.h"
#include "util.h"
#include <stdint.h>



static EmitStatus_t mov_rDQ_mem_nospbp(CodeBuffer_t* codebuffer, byte_t lhs, SizeReference_t lhs_szref, Register64_t rhs_base, int32_t rhs_offset) {

    union {
        int32_t i32;
        byte_t buf[4];
    } u;

    u.i32 = rhs_offset;

    byte_t buf[] = {
        (byte_t)REX::Base,
        0x8b, // mov opcode
        0x00, // ModRM may change to 0x40 or 0x80
        u.buf[0], u.buf[1], u.buf[2], u.buf[3]
    };

    byte_t* start_iter = buf + 1; // start with 32-bit destination
    byte_t* modrm_ptr  = buf + 2;
    byte_t* end_iter   = buf + 3; // start with offset of 0

    if(rhs_offset == 0) {
        ; // nothing to adjust
    } else if(rhs_offset >= -128 && rhs_offset < 128) {
        *modrm_ptr = 0x40;
        end_iter = buf + 4;
    } else { // assume 32-bit offset here
        *modrm_ptr = 0x80;
        end_iter = buf + 7;
    }

    // get dest and base into ModRM byte
    *modrm_ptr |= (byte_t)((lhs << 3) & 0b00111000);
    *modrm_ptr |= (byte_t)(rhs_base & 0b0111);

    switch(lhs_szref) {
    case SizeReference_t::_32b:
        if(lhs >= (byte_t)Register32_t::R8d) {
            start_iter = buf;
            buf[0] |= (byte_t)REX::R;
        }
        if(rhs_base >= Register64_t::R8) {
            start_iter = buf;
            buf[0] |= (byte_t)REX::B;
        }
        break;

    case SizeReference_t::_64b:
        start_iter = buf; // always need REX if 64-bit dest
        buf[0] |= (byte_t)REX::W;
        if(rhs_base >= Register64_t::R8)
            buf[0] |= (byte_t)REX::B;
        break;
    }

    CodeBuffer_emit_raw(codebuffer, start_iter, end_iter);
    return EmitStatus_t::SUCCESS;
}

static EmitStatus_t mov_r64_mem_rsp(CodeBuffer_t* codebuffer, Register64_t lhs, int32_t rhs_offset) {

    i32_buffer_t b = i32_to_buffer(rhs_offset);

    byte_t buf[] = {
        (byte_t)(0x48 | (byte_t)(lhs >= Register64_t::R8 ? (byte_t)REX::R : 0x00)), // REX.w[r]
        0x8b, // opcode
        (byte_t)(0x04 | (byte_t)((lhs << 3) & 0b00111000)), // ModRM
        0x24, // SIB?
        b[0], b[1], b[2], b[3]
    };

    byte_t* start_iter = buf;
    byte_t* end_iter   = buf + 4;

    if(rhs_offset == 0) {
        CodeBuffer_emit_raw(codebuffer, start_iter, end_iter);
        return EmitStatus_t::SUCCESS;
    }

    SizeReference_t offset_sz = offset_size_i32(rhs_offset);

    buf[2] |= (offset_sz == SizeReference_t::_8b ? 0x40 : 0x80);
    end_iter = offset_sz == SizeReference_t::_8b ? buf + 5 : buf + 8;

    CodeBuffer_emit_raw(codebuffer, start_iter, end_iter);
    return EmitStatus_t::SUCCESS;
}

static EmitStatus_t mov_r64_mem_rbp(CodeBuffer_t* codebuffer, Register64_t lhs, int32_t rhs_offset) {

    i32_buffer_t b = i32_to_buffer(rhs_offset);

    byte_t buf[] = {
        (byte_t)(0x48 | (lhs >= Register64_t::R8 ? (byte_t)REX::R : 0x00)), // REX.w[r]
        0x8b,
        (byte_t)(0x45 | (byte_t)((lhs << 3) & 0b00111000)), // ModRM
        b[0], b[1], b[2], b[3]
    };

    SizeReference_t offset_sz = offset_size_i32(rhs_offset);

    buf[2] |= (offset_sz == SizeReference_t::_8b ? 0x40 : 0x80);
    byte_t* end_iter = (offset_sz == SizeReference_t::_8b) ? buf + 4 : buf + 7;

    CodeBuffer_emit_raw(codebuffer, buf, end_iter);
    return EmitStatus_t::SUCCESS;
}

EmitStatus_t mov_r64_mem(CodeBuffer_t* codebuffer, Register64_t lhs, Register64_t rhs_base, int32_t rhs_offset) {
    switch(rhs_base) {
    case Register64_t::RSP: return mov_r64_mem_rsp(codebuffer, lhs, rhs_offset);
    case Register64_t::RBP: return mov_r64_mem_rbp(codebuffer, lhs, rhs_offset);
    default:                return mov_rDQ_mem_nospbp(codebuffer, (byte_t)lhs, SizeReference_t::_64b, rhs_base, rhs_offset);
    }
}

// TODO : reimplement, i got some stuff wrong here
EmitStatus_t mov_r64_r64(CodeBuffer_t* codebuffer, Register64_t lhs, Register64_t rhs) {

    byte_t buf[] = {
        0x48, // REX + w
        0x89, // mov opcode
        0xc0  // ModRM
    };

    buf[2] |= ((rhs << 3) & 0b00111000);
    buf[2] |= (lhs & 0b0111);

    if(lhs >= Register64_t::R8)
        buf[0] |= (byte_t)REX::B;

    if(rhs >= Register64_t::R8)
        buf[0] |= (byte_t)REX::R;

    CodeBuffer_emit_raw(codebuffer, buf, buf+3);
    return EmitStatus_t::SUCCESS;
}

