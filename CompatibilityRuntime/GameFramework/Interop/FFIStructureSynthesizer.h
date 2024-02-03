#ifndef TRANSLATOR_FFI_STRUCTURE_SYNTHESIZER_H
#define TRANSLATOR_FFI_STRUCTURE_SYNTHESIZER_H

#include <shared_mutex>
#include <unordered_map>
#include <memory>

#include <ffi.h>

#include <il2cpp-api.h>

class FFISynthesizedStructure;

class FFIStructureSynthesizer {
public:
    FFIStructureSynthesizer();
    ~FFIStructureSynthesizer();

    FFIStructureSynthesizer(const FFIStructureSynthesizer &other) = delete;
    FFIStructureSynthesizer &operator =(const FFIStructureSynthesizer &other) = delete;

    ffi_type *createFFITypeForValueType(Il2CppClass *valueTypeClass);

private:
    std::shared_mutex m_mutex;
    std::unordered_map<Il2CppClass *, std::unique_ptr<FFISynthesizedStructure>> m_structures;
};

#endif

