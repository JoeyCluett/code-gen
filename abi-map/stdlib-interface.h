#pragma once

//
// defines JIT-compiled stdlib interfaces
//

#include <abi-map/signature.h>

void stdlib_interface_init(void);

void call_stdlib(const std::string& stdlib_fn_name, const std::vector<NativeType_t>* args);

