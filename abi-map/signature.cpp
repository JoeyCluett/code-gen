#include "signature.h"

#include <x86_emit/xmm/movfp.h>
#include <x86_emit/mov.h>
#include <x86_emit/jmp.h>

#include <stdexcept>
#include <vector>
#include <string>
#include <stddef.h>

NativeType_t::NativeType_t(void* vptr)       { this->type = NativeType_t::Type_t::ptr_gen;  this->vptr = vptr; }
NativeType_t::NativeType_t(const char* cptr) { this->type = NativeType_t::Type_t::ptr_gen;  this->vptr = (void*)cptr; }
NativeType_t::NativeType_t(int8_t i8)    { this->type = NativeType_t::Type_t::_8b_int;  this->i8 = i8;     }
NativeType_t::NativeType_t(int16_t i16)  { this->type = NativeType_t::Type_t::_16b_int; this->i16 = i16;   }
NativeType_t::NativeType_t(int32_t i32)  { this->type = NativeType_t::Type_t::_32b_int; this->i32 = i32;   }
NativeType_t::NativeType_t(int64_t i64)  { this->type = NativeType_t::Type_t::_64b_int; this->i64 = i64;   }
NativeType_t::NativeType_t(uint8_t u8)   { this->type = NativeType_t::Type_t::_8b_int;  this->u8 = u8;     }
NativeType_t::NativeType_t(uint16_t u16) { this->type = NativeType_t::Type_t::_16b_int; this->u16 = u16;   }
NativeType_t::NativeType_t(uint32_t u32) { this->type = NativeType_t::Type_t::_32b_int; this->u32 = u32;   }
NativeType_t::NativeType_t(uint64_t u64) { this->type = NativeType_t::Type_t::_64b_int; this->u64 = u64;   }
NativeType_t::NativeType_t(float f32)    { this->type = NativeType_t::Type_t::_32b_fp;  this->f32 = f32;   }
NativeType_t::NativeType_t(double f64)   { this->type = NativeType_t::Type_t::_64b_fp;  this->f64 = f64;   }

NativeInterface_t generate_native_interface(const std::string& signature) {

    //
    // follows the SysV ABI for x86-64 machines
    //
    // rdi : pointer to array of NativeType_t's
    // rsi : function to call
    //
    // NOTE : this function currently supports at most 6 integer types and 8 floating point types
    //

    NativeInterface_t ni;
    ni.signature = signature;

    const size_t native_stride      = sizeof(NativeType_t);
    const size_t native_type_offset = offsetof(NativeType_t, vptr);

    mov_r64_r64(&ni.cb, Register64_t::RAX, Register64_t::RDI); // move NativeType_t pointer to temporary
    mov_r64_r64(&ni.cb, Register64_t::R11, Register64_t::RSI); // move fptr to temporary

    Register64_t abi_ordering_i64[] = {
        Register64_t::RDI,
        Register64_t::RSI,
        Register64_t::RDX,
        Register64_t::RCX,
        Register64_t::R8,
        Register64_t::R9,
    };

    Register32_t abi_ordering_i32[] = {
        Register32_t::EDI,
        Register32_t::ESI,
        Register32_t::EDX,
        Register32_t::ECX,
        Register32_t::R8d,
        Register32_t::R9d,
    };

    XMM_t abi_ordering_fp[] = {
        XMM_t::XMM0,
        XMM_t::XMM1,
        XMM_t::XMM2,
        XMM_t::XMM3,
        XMM_t::XMM4,
        XMM_t::XMM5,
        XMM_t::XMM6,
        XMM_t::XMM7,
    };

    size_t r_select = 0ul;
    size_t fp_select  = 0ul;
    size_t current_native_type_offset = native_type_offset;

    for(const char c : signature) {

        switch(c) {
        case 'p': // pointer type, this is identical to integer type
        case 'I': // 64-bit integer type (doesnt matter which it is)
            if(r_select >= 6)
                throw std::runtime_error("signature requires more than 6 integer-type arguments");

            mov_r64_mem(&ni.cb, abi_ordering_i64[r_select], Register64_t::RAX, current_native_type_offset);
            r_select++;
            break;

        case 'i': // 32-bit integer type
            if(r_select >= 6)
                throw std::runtime_error("signature requires more than 6 integer-type arguments");

            //mov_r32_mem(&ni.cb, abi_ordering_i32[r_select], Register64_t::RAX, current_native_type_offset);
            mov_r64_mem(&ni.cb, abi_ordering_i64[r_select], Register64_t::RAX, current_native_type_offset);
            r_select++;
            break;

        case 'f': // 32-bit float
            if(fp_select >= 8)
                throw std::runtime_error("signature requires more than 8 single/double floating-point arguments");

            movss_xmm_mem(&ni.cb, abi_ordering_fp[fp_select], Register64_t::RAX, current_native_type_offset);
            fp_select++;
            break;

        case 'F': // 64-bit float type
            if(fp_select >= 8)
                throw std::runtime_error("signature requires more than 8 single/double floating-point arguments");

            movsd_xmm_mem(&ni.cb, abi_ordering_fp[fp_select], Register64_t::RAX, current_native_type_offset);
            fp_select++;
            break;

        default:
            throw std::runtime_error("invalid type in signature string");
        }

        current_native_type_offset += native_stride; // advance pointer to next native type
    }

    // jump using the temporary register from earlier
    jmp_r64(&ni.cb, Register64_t::R11);

    return ni;
}

void call_native_function(NativeInterface_t* ni, const std::vector<NativeType_t>* args, void* native_fptr) {
    NativeInterfaceFptr native_interface_fptr = (NativeInterfaceFptr)ni->cb.executable_fptr;
    native_interface_fptr(args->data(), native_fptr);
}

const bool NativeType_is_integer_type(const NativeType_t& nt) {

    switch(nt.type) {
    case NativeType_t::Type_t::ptr_gen:
    case NativeType_t::Type_t::_8b_int:
    case NativeType_t::Type_t::_16b_int:
    case NativeType_t::Type_t::_32b_int:
    case NativeType_t::Type_t::_64b_int:
        return true;
    default: return false;
    }

}


