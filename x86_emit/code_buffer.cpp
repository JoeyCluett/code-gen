#include "code_buffer.h"

#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <stdexcept>
#include <iostream>

void CodeBuffer_emit_raw(CodeBuffer_t* buf, unsigned char* begin, unsigned char* end) {
    buf->code.insert(buf->code.end(), begin, end);
}

void CodeBuffer_write_code_to_file(CodeBuffer_t* buf, const std::string& filename) {
    FILE* fptr = fopen(filename.c_str(), "wb"); // truncate
    fwrite(buf->code.data(), 1, buf->code.size(), fptr);
    fclose(fptr);
}

void CodeBuffer_generate_executable(CodeBuffer_t* buf) {

    void* exec_ptr = mmap(
        NULL,                        // kernel can put this memory anywhere it wants
        buf->code.size(),            // space we need
        PROT_READ | PROT_WRITE,      // read and write, later this will become PROT_EXEC
        MAP_PRIVATE | MAP_ANONYMOUS, // other procs dont need to see this memory, memory doesnt need to correspond to a file
        -1,                          // not mapped to a file
        0);                          // start is at the beginning of mmap'ed memory

    if(exec_ptr == (void*)-1) {
        throw std::runtime_error("mmap alloc failed");
    }

    buf->executable_fptr = exec_ptr;
    buf->alloced_size    = buf->code.size();

    memcpy(exec_ptr, buf->code.data(), buf->alloced_size);
    mprotect(exec_ptr, buf->alloced_size, PROT_READ | PROT_EXEC);

    std::cout << "generated fptr address : " << buf->executable_fptr << std::endl;
}

void CodeBuffer_free_exec_memory(CodeBuffer_t* buf) {
    munmap(buf->executable_fptr, buf->alloced_size);
    buf->executable_fptr = NULL;
    buf->alloced_size    = 0ul;
}
