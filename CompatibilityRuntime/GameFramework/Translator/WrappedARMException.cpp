#include <Translator/WrappedARMException.h>

WrappedARMException::WrappedARMException(uint64_t exceptionClass, void *exceptionObject) :
    m_exceptionClass(exceptionClass), m_exceptionObject(exceptionObject) {

}

WrappedARMException::~WrappedARMException() = default;

WrappedARMException::WrappedARMException(const WrappedARMException &other) noexcept = default;

WrappedARMException &WrappedARMException::operator =(const WrappedARMException &other) noexcept = default;

