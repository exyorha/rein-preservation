#include <VideoPlayer/MPVError.h>
#include <VideoPlayer/MPVTypes.h>

MPVError::MPVError(int error) : m_error(error) {

}

MPVError::~MPVError() = default;

MPVError::MPVError(const MPVError &other) noexcept = default;

MPVError &MPVError::operator =(const MPVError &other) noexcept = default;

const char* MPVError::what() const noexcept {
    return DynamicallyLinkedMPV::get().mpv_error_string(m_error);
}
