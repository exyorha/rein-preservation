#include <Interop/FFIStructureSynthesizer.h>
#include <Interop/FFISynthesizedStructure.h>

#include <mutex>

FFIStructureSynthesizer::FFIStructureSynthesizer() = default;

FFIStructureSynthesizer::~FFIStructureSynthesizer() = default;

ffi_type *FFIStructureSynthesizer::createFFITypeForValueType(Il2CppClass *valueTypeClass) {
    {
        std::shared_lock<std::shared_mutex> locker(m_mutex);
        auto it = m_structures.find(valueTypeClass);
        if(it != m_structures.end()) {
            return it->second.get();
        }
    }

    auto structure = std::make_unique<FFISynthesizedStructure>(valueTypeClass);
    {
        std::unique_lock<std::shared_mutex> locker(m_mutex);

        auto result = m_structures.emplace(valueTypeClass, std::move(structure));
        /*
         * It's possible that someone else has already built this exact type
         * in the time window when the mutex was unlocked. It's fine, we've
         * just wasted a bit of work and can return the type that's already
         * here.
         */
        return result.first->second.get();
    }
}
