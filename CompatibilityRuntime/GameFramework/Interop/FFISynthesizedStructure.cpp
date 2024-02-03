#include <Interop/FFISynthesizedStructure.h>

#include <array>
#include <ffi.h>
#include <stdexcept>

FFISynthesizedStructure::FFISynthesizedStructure(Il2CppClass *typeClass) {
    uint32_t valueAlign;
    auto valueSize = il2cpp_class_value_size(typeClass, &valueAlign);

    if(valueSize == 0)
        throw std::logic_error("zero-sized structure");

    if(valueSize < valueAlign)
        throw std::logic_error("structure size is less than the structure alignment");

    /*
     * Rough estimate that should work in most cases.
     */
    m_elements.reserve(valueSize / valueAlign + 1);

    /*
     * First, determine the largest value size we can use to maintain the alignment.
     */
    if(valueAlign != 8 && valueAlign != 4 && valueAlign != 2 && valueAlign != 1) {
        throw std::logic_error("unsupported structure alignment: " + std::to_string(valueAlign));
    }

    auto remainingValueSize = valueSize;

    for(ffi_type *typeToTry: { &ffi_type_uint64, &ffi_type_uint32, &ffi_type_uint16, &ffi_type_uint8 }) {
        while(valueAlign >= typeToTry->alignment && remainingValueSize >= typeToTry->size) {
            m_elements.emplace_back(typeToTry);
            remainingValueSize -= typeToTry->size;
        }
    }

    if(remainingValueSize != 0)
        throw std::logic_error("the structure has not been completely filled");

    m_elements.push_back(nullptr);

    this->type = FFI_TYPE_STRUCT;
    this->alignment = 0;
    this->size = 0;
    this->elements = m_elements.data();

    /*
     * Prepare a dummy cif to make sure that the type we've built is
     * initialized *now*, before it's used - this avoids concurrency
     * issues later.
     */
    std::array<ffi_type *, 1> args{ this };

    ffi_cif dummy_cif;
    auto result = ffi_prep_cif(&dummy_cif, FFI_DEFAULT_ABI, args.size(), &ffi_type_void, args.data());
    if(result != FFI_OK)
        throw std::runtime_error("the dummy ffi_prep_cif has failed");

    if(this->size != valueSize || this->alignment != valueAlign) {
        throw std::logic_error("the structure size and alignnment didn't match the expected values after building the FFI type");
    }
}

FFISynthesizedStructure::~FFISynthesizedStructure() = default;
