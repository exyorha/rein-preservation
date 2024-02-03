#ifndef INTEROP_FFI_SYNTHESIZED_STRUCTURE_H
#define INTEROP_FFI_SYNTHESIZED_STRUCTURE_H

#include <ffi.h>

#include <il2cpp-api.h>

#include <vector>

class FFISynthesizedStructure final : public ffi_type {
public:
    explicit FFISynthesizedStructure(Il2CppClass *classDesc);
    ~FFISynthesizedStructure();

    FFISynthesizedStructure(const FFISynthesizedStructure &other) = delete;
    FFISynthesizedStructure &operator =(const FFISynthesizedStructure &other) = delete;

private:
    std::vector<ffi_type *> m_elements;
};

#endif
