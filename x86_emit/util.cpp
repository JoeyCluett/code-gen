#include "util.h"

#include <stdint.h>

SizeReference_t offset_size_i32(int32_t i32) {
    if(i32 >= -128 && i32 < 128) {
        return SizeReference_t::_8b;
    } else {
        return SizeReference_t::_32b;
    }
}

i32_buffer_t i32_to_buffer(int32_t i32) {

    union {
        int32_t i32;
        byte_t buf[4];
    } u;

    u.i32 = i32;

    return { u.buf[0], u.buf[1], u.buf[2], u.buf[3] };
}
