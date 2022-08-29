#include "jmp.h"

EmitStatus_t jmp_r64(CodeBuffer_t* codebuffer, Register64_t dest) {

    byte_t buf[] = {
        0x41,
        0xff,
        0xe0
    };

    buf[2] |= (byte_t)(dest & 0b0111);

    if(dest >= Register64_t::R8) {
        CodeBuffer_emit_raw(codebuffer, buf, buf + 3);
    } else {
        CodeBuffer_emit_raw(codebuffer, buf + 1, buf + 3);
    }

    return EmitStatus_t::SUCCESS;
}
