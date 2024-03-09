#ifndef DATAMODEL_ZLIB_EXCEPTION_H
#define DATAMODEL_ZLIB_EXCEPTION_H

#include <stdexcept>

class ZlibStream;

class ZlibException final : public std::runtime_error {
public:
    explicit ZlibException(int error);
    ZlibException(int zlibError, const ZlibStream &stream);
    ~ZlibException();

    ZlibException(const ZlibException &other) noexcept;
    ZlibException &operator =(const ZlibException &other) noexcept;

private:
    static std::string composeError(int zlibError);
    static std::string composeError(int zlibError, const ZlibStream &stream);
};

#endif
