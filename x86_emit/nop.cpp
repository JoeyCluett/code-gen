#include "nop.h"

#include "code_buffer.h"
#include "util.h"

#include <stdint.h>

EmitStatus_t _nop(CodeBuffer_t* codebuffer) {
    byte_t buf[1] = { 0x90 };
    CodeBuffer_emit_raw(codebuffer, buf, buf + 1);
    return EmitStatus_t::SUCCESS;
}

EmitStatus_t _nop_N(CodeBuffer_t* codebuffer, int nop_bytes) {

    byte_t nop1[1] = { 0x90 };
    byte_t nop2[2] = { 0x66, 0x90 };
    byte_t nop3[3] = { 0x0F, 0x1F, 0x00 };
    byte_t nop4[4] = { 0x0F, 0x1F, 0x40, 0x00 };
    byte_t nop5[5] = { 0x0F, 0x1F, 0x44, 0x00, 0x00 };
    byte_t nop6[6] = { 0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00 };
    byte_t nop7[7] = { 0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00 };
    byte_t nop8[8] = { 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00 };
    byte_t nop9[9] = { 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00 };

    while(nop_bytes >= 9) {
        CodeBuffer_emit_raw(codebuffer, nop9, nop9 + 9);
        nop_bytes -= 9;
    }

    switch(nop_bytes) {
    case 0: return EmitStatus_t::SUCCESS;
    case 1: CodeBuffer_emit_raw(codebuffer, nop1, nop1 + 1); return EmitStatus_t::SUCCESS;
    case 2: CodeBuffer_emit_raw(codebuffer, nop2, nop2 + 2); return EmitStatus_t::SUCCESS;
    case 3: CodeBuffer_emit_raw(codebuffer, nop3, nop3 + 3); return EmitStatus_t::SUCCESS;
    case 4: CodeBuffer_emit_raw(codebuffer, nop4, nop4 + 4); return EmitStatus_t::SUCCESS;
    case 5: CodeBuffer_emit_raw(codebuffer, nop5, nop5 + 5); return EmitStatus_t::SUCCESS;
    case 6: CodeBuffer_emit_raw(codebuffer, nop6, nop6 + 6); return EmitStatus_t::SUCCESS;
    case 7: CodeBuffer_emit_raw(codebuffer, nop7, nop7 + 7); return EmitStatus_t::SUCCESS;
    case 8: CodeBuffer_emit_raw(codebuffer, nop8, nop8 + 8); return EmitStatus_t::SUCCESS;
    case 9: CodeBuffer_emit_raw(codebuffer, nop9, nop9 + 9); return EmitStatus_t::SUCCESS;
    default: return EmitStatus_t::FAILURE;
    }    

}
