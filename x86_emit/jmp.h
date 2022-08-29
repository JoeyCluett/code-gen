#pragma once

#include "code_buffer.h"
#include "util.h"

#include <stdint.h>

EmitStatus_t jmp_r64(CodeBuffer_t* codebuffer, Register64_t dest);
