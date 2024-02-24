#ifndef TRANSLATOR_WRAPPED_ARM_EXCEPTION_H
#define TRANSLATOR_WRAPPED_ARM_EXCEPTION_H

#include <cstdint>

/*
 * TODO: exception objects that were handled locally (without handing
 * them over to _Unwind_RaiseException again) need to be freed.
 */

class WrappedARMException {
public:
    explicit WrappedARMException(uint64_t exceptionClass, void *exceptionObject);
    ~WrappedARMException();

    WrappedARMException(const WrappedARMException &other) noexcept;
    WrappedARMException &operator =(const WrappedARMException &other) noexcept;

    inline void *exceptionObject() const {
        return m_exceptionObject;
    }

private:
    uint64_t m_exceptionClass;
    void *m_exceptionObject;
};

#endif
