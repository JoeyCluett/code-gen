#pragma once

#include <x86_emit/code_buffer.h>

#include <string>
#include <vector>
#include <stdint.h>

struct NativeType_t {

    enum Type_t {
        ptr_gen,
        _8b_int,
        _16b_int,
        _32b_int,
        _64b_int,
        _32b_fp,
        _64b_fp,
    };

    Type_t type;

    union {
        void* vptr;
        
        int8_t  i8;
        uint8_t u8;
        int16_t  i16;
        uint16_t u16;
        int32_t  i32;
        uint32_t u32;
        int64_t  i64;
        uint64_t u64;

        float f32;
        double f64;
    };

    NativeType_t(void* vptr);
    NativeType_t(const char* vptr);

    NativeType_t(int8_t i8);
    NativeType_t(int16_t i8);
    NativeType_t(int32_t i8);
    NativeType_t(int64_t i8);
    NativeType_t(uint8_t i8);
    NativeType_t(uint16_t i8);
    NativeType_t(uint32_t i8);
    NativeType_t(uint64_t i8);
    NativeType_t(float f32);
    NativeType_t(double f64);

};

struct NativeInterface_t {
    CodeBuffer_t cb;
    std::string signature;
};

typedef void(*NativeInterfaceFptr)(const NativeType_t* args, void* fptr);

//
// uses string to generate native interface stub
//
// signature rules:
//  'p' : generic pointer type
//  'i' : 32-bit integer type
//  'I' : 64-bit integer type
//  'f' : single-precision floating point type
//  'F' : double-precision floating point type
//
NativeInterface_t generate_native_interface(const std::string& signature);

void call_native_function(NativeInterface_t* callable_code, const std::vector<NativeType_t>* args, void* fptr);

const bool NativeType_is_integer_type(const NativeType_t& nt);
