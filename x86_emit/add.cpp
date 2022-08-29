#include "add.h"
#include "code_buffer.h"
#include "util.h"

static EmitStatus_t add_rDQ_mem(
        CodeBuffer_t* codebuffer, byte_t lhs, SizeReference_t lhs_szref, 
        Register64_t rhs_base, int32_t rhs_off) {

    union {
        int32_t i32;
        byte_t  buf[4];
    } u;

    u.i32 = rhs_off;

    byte_t buf[7] = {
        (byte_t)REX::Base,
        0x30, // <-- opcode
        0x00, 
        u.buf[0], u.buf[1], u.buf[2], u.buf[3] }; // <-- max size of offset is 4 bytes

    byte_t* begin = buf + 1;
    byte_t* end   = buf + 3;

    if(rhs_off == 0) { // offset of zero
        // no change to end iter
    } else if(rhs_off >= -128 && rhs_off <= 127) { // 8-bit offset
        // end iter must extend an additional byte
        end = buf + 4;
        buf[2] = 0x40; // mark as 8-bit offset
    } else { // promote to 32-bit offset
        // end iter must extend 4 additional bytes
        end = buf + 7;
        buf[2] = 0x80; // mark as 32-bit offset
    }

    buf[2] |= (byte_t)((lhs << 3) & 0b00111000);
    buf[2] |= (byte_t)(rhs_base & 0b00000111);

    // if the operands are sufficiently small, we can emit here and call it good
    if(lhs_szref == SizeReference_t::_32b && lhs < Register32_t::R8d && rhs_base < Register64_t::R8) {
        CodeBuffer_emit_raw(codebuffer, begin, end);
        return EmitStatus_t::SUCCESS;
    }

    begin = buf; // need to include the REX flag byte

    if(
            (lhs_szref == SizeReference_t::_32b && lhs > Register32_t::EDI) ||
            (lhs_szref == SizeReference_t::_64b && lhs > Register64_t::RDI))
        buf[0] |= (byte_t)REX::R;

    if(rhs_base > Register64_t::RDI)
        buf[0] |= (byte_t)REX::B;

    CodeBuffer_emit_raw(codebuffer, begin, end);
    return EmitStatus_t::SUCCESS;

}

EmitStatus_t add_r32_mem(CodeBuffer_t* codebuffer, Register32_t lhs, Register64_t rhs_base, int32_t rhs_off) {
    return add_rDQ_mem(codebuffer, (byte_t)lhs, SizeReference_t::_32b, rhs_base, rhs_off);
}

EmitStatus_t add_r64_mem(CodeBuffer_t* codebuffer, Register64_t lhs, Register64_t rhs_base, int32_t rhs_off) {
    return add_rDQ_mem(codebuffer, (byte_t)lhs, SizeReference_t::_64b, rhs_base, rhs_off);
}

EmitStatus_t add_r8_r8(CodeBuffer_t* codebuffer, Register8_t lhs, Register8_t rhs) {

    byte_t buf[2] = { 0x00, 0xc0 };

    buf[1] |= (byte_t)lhs;
    buf[1] |= (byte_t)(rhs << 3);

    CodeBuffer_emit_raw(codebuffer, buf, buf + 2);
    return EmitStatus_t::SUCCESS;
}

EmitStatus_t add_r16_r16(CodeBuffer_t* codebuffer, Register16_t lhs, Register16_t rhs) {

    byte_t buf[3] = { 0x66, 0x01, 0xc0 };

    buf[2] |= (byte_t)lhs;
    buf[2] |= (byte_t)(rhs << 3);

    CodeBuffer_emit_raw(codebuffer, buf, buf + 3);
    return EmitStatus_t::SUCCESS;
}

EmitStatus_t add_r32_r32(CodeBuffer_t* codebuffer, Register32_t lhs, Register32_t rhs) {

    byte_t buf[2] = { 0x01, 0xc0 };

    buf[1] |= (byte_t)(lhs & 0b00000111);
    buf[1] |= (byte_t)((rhs << 3) & 0b00111000);

    CodeBuffer_emit_raw(codebuffer, buf, buf + 2);
    return EmitStatus_t::SUCCESS;
}

EmitStatus_t add_r64_r64(CodeBuffer_t* codebuffer, Register64_t lhs, Register64_t rhs) {

    byte_t buf[3] = { 0x48, 0x01, 0xc0 };

    if(lhs > Register64_t::RDI)
        buf[0] |= (byte_t)REX::B;
    
    if(rhs > Register64_t::RDI)
        buf[0] |= (byte_t)REX::R;

    buf[2] |= (byte_t)(lhs & 0b00000111);
    buf[2] |= (byte_t)((rhs << 3) & 0b00111000);

    CodeBuffer_emit_raw(codebuffer, buf, buf + 3);
    return EmitStatus_t::SUCCESS;
}




