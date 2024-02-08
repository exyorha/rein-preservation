#include "JSONWriter.h"

JSONWriter::JSONWriter() : m_context(yajl_gen_alloc(nullptr)) {

    if(!m_context)
        throw std::bad_alloc();
}

JSONWriter::~JSONWriter() = default;

void JSONWriter::writeInteger(long long number) {
    checkAndThrow(yajl_gen_integer(m_context.get(), number));
}

void JSONWriter::writeDouble(double number) {
    checkAndThrow(yajl_gen_double(m_context.get(), number));
}

void JSONWriter::writeNumber(const std::string_view &numberRepr) {
    checkAndThrow(yajl_gen_number(m_context.get(), numberRepr.data(), numberRepr.size()));
}

void JSONWriter::writeString(const std::string_view &string) {
    checkAndThrow(yajl_gen_string(m_context.get(), reinterpret_cast<const unsigned char *>(string.data()), string.size()));
}

void JSONWriter::writeNull() {
    checkAndThrow(yajl_gen_null(m_context.get()));
}

void JSONWriter::writeBool(bool value) {
    checkAndThrow(yajl_gen_bool(m_context.get(), value));
}

void JSONWriter::writeMapOpen() {
    checkAndThrow(yajl_gen_map_open(m_context.get()));
}

void JSONWriter::writeMapClose() {
    checkAndThrow(yajl_gen_map_close(m_context.get()));
}

void JSONWriter::writeArrayOpen() {
    checkAndThrow(yajl_gen_array_open(m_context.get()));
}

void JSONWriter::writeArrayClose() {
    checkAndThrow(yajl_gen_array_close(m_context.get()));
}

void JSONWriter::throwException(yajl_gen_status status) {
    throw std::runtime_error("a yajl error has occurred while writing JSON: " + std::to_string(status));
}

std::string_view JSONWriter::output() const {
    const unsigned char *buf;
    size_t size;

    checkAndThrow(yajl_gen_get_buf(m_context.get(), &buf, &size));

    return std::string_view(reinterpret_cast<const char *>(buf), size);
}

void JSONWriter::clear() {
    yajl_gen_clear(m_context.get());
}

void JSONWriter::reset(const char *separator) {
    yajl_gen_reset(m_context.get(), separator);
}
