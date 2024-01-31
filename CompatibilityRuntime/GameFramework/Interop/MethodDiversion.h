#ifndef INTEROP_METHOD_DIVERSION_H
#define INTEROP_METHOD_DIVERSION_H

#include <il2cpp-api.h>
#include <ffi.h>

#include <Interop/ARMArgumentPacker.h>

struct Diversion;

class MethodDiversion {
public:
    MethodDiversion(const MethodInfo *method, Il2CppMethodPointer interposer);
    ~MethodDiversion();

    MethodDiversion(const MethodDiversion &other) = delete;
    MethodDiversion &operator =(const MethodDiversion &other) = delete;

    static void diversionHandler(const Diversion *diversion);

private:
    class AllocatedClosure {
    public:
        explicit AllocatedClosure(size_t size = sizeof(ffi_closure));
        ~AllocatedClosure();

        AllocatedClosure(const AllocatedClosure &other) = delete;
        AllocatedClosure &operator =(const AllocatedClosure &other) = delete;

        inline ffi_closure *closure() const {
            return static_cast<ffi_closure *>(m_closureData);
        }

        inline void *code() const {
            return m_closureCode;
        }

    private:
        void *m_closureData;
        void *m_closureCode;
    };

    static void continueCall(ffi_cif *cif, void *ret, void **args, void *userData);

    AllocatedClosure m_closure;
    Il2CppMethodPointer m_interposer;
    ARMArgumentSet m_arguments;
    ARMSingleArgument m_result;
    ffi_cif m_cifForInterposerCall;
    ffi_cif m_cifForContinueCall;
};

#endif
