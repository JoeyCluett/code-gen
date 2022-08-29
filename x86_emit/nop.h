#pragma once

#include "code_buffer.h"
#include "util.h"

#include <stdint.h>

//
// single byte nop
//
EmitStatus_t _nop(CodeBuffer_t* codebuffer);

//
// pad with arbitrary number of nops
//
EmitStatus_t _nop_N(CodeBuffer_t* codebuffer, int nop_bytes);


