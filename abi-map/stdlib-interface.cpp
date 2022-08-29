#include "stdlib-interface.h"

#include <abi-map/signature.h>

#include <string>
#include <map>
#include <stdexcept>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#define STDLIB_ENTRY(fn_name, signature) { #fn_name, { (void*)fn_name, generate_native_interface(signature) } }

static std::map<std::string, std::pair<void*, NativeInterface_t> > stdlib_ni_lookup = {
    STDLIB_ENTRY(puts, "p"),
};

#undef STDLIB_ENTRY

void stdlib_interface_init(void) {
    for(auto& iter : stdlib_ni_lookup) {
        CodeBuffer_generate_executable(&iter.second.second.cb);
    }
}

void call_stdlib(const std::string& stdlib_fn_name, const std::vector<NativeType_t>* args) {

    auto iter = stdlib_ni_lookup.find(stdlib_fn_name);
    if(iter == stdlib_ni_lookup.end()) {
        throw std::runtime_error("stdlib function `" + stdlib_fn_name + "` doesnt have a generated native interface");
    }

    call_native_function((NativeInterface_t*)&iter->second.second, args, iter->second.first);
}
