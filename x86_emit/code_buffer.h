#pragma once

#include <vector>
#include <string>

struct CodeBuffer_t {

    std::vector<unsigned char> code;
    // TODO : jump tables

    void* executable_fptr;
    size_t alloced_size;
};

void CodeBuffer_emit_raw(CodeBuffer_t* buf, unsigned char* begin, unsigned char* end);

//
// writes generated code bytes to file
//
void CodeBuffer_write_code_to_file(CodeBuffer_t* buf, const std::string& filename);

//
// uses mmap to create executable memory with same code given in buf->code
//
void CodeBuffer_generate_executable(CodeBuffer_t* buf);

//
// calls munmap on allocated executable memory
//
void CodeBuffer_free_exec_memory(CodeBuffer_t* buf);

