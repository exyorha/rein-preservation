#include <DataModel/ZlibException.h>
#include <DataModel/ZlibStream.h>

ZlibException::ZlibException(int zlibError) : std::runtime_error(composeError(zlibError)) {

}

ZlibException::ZlibException(int zlibError, const ZlibStream &stream) : std::runtime_error(composeError(zlibError, stream)) {

}

ZlibException::~ZlibException() = default;

ZlibException::ZlibException(const ZlibException &other) noexcept = default;

ZlibException &ZlibException::operator =(const ZlibException &other) noexcept = default;

std::string ZlibException::composeError(int zlibError) {
    return zError(zlibError);
}

std::string ZlibException::composeError(int zlibError, const ZlibStream &stream) {
    if(stream.msg)
        return stream.msg;
    else
        return zError(zlibError);
}
