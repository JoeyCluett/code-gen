#include <abi-map/signature.h>
#include <abi-map/stdlib-interface.h>

#include <iostream>

void sample_function(float* result, float a, float b, float c, float d);

int main(void) {

    auto ni = generate_native_interface("pffff");

    CodeBuffer_generate_executable(&ni.cb);

    float result;

    std::vector<NativeType_t> args = { &result, 1.0f, 2.0f, 3.0f, 4.0f };
    call_native_function(&ni, &args, (void*)sample_function);
    std::cout << "result : " << result << std::endl;

    args = { &result, 3.14159f, 1.0f, 1.0f, 1.0f };
    call_native_function(&ni, &args, (void*)sample_function);
    std::cout << "result : " << result << std::endl;

    //CodeBuffer_write_code_to_file(&ni.cb, "output.txt");

    stdlib_interface_init();
    std::vector<NativeType_t> stdlib_args = { "hello from wrapped stdlib `puts`" };
    call_stdlib("puts", &stdlib_args);

    return 0;
}

void sample_function(float* result, float a, float b, float c, float d) {
    *result = a * b * c * d * 1.3f;
}
