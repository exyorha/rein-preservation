#include <GRPC/PBOutputOverLLBuffer.h>

PBOutputOverLLBuffer::PBOutputOverLLBuffer(evbuffer *buffer) : m_buffer(buffer), m_position(0) {

}

PBOutputOverLLBuffer::~PBOutputOverLLBuffer() {
    if(m_reservation.has_value()) {
        BackUp(0);
    }
}

bool PBOutputOverLLBuffer::Next(void** data, int* size) {
    if(m_reservation.has_value()) {
        BackUp(0);
    }

    evbuffer_iovec reservation;
    if(evbuffer_reserve_space(m_buffer, 8192, &reservation, 1) < 0)
        throw std::runtime_error("evbuffer_reserve_space has failed");

    *data = reservation.iov_base;
    *size = reservation.iov_len;

    m_reservation.emplace(reservation);

    return true;
}

void PBOutputOverLLBuffer::BackUp(int count) {
    if(count == 0 && !m_reservation.has_value())
        return;

    auto &reservation = m_reservation.value();

    if(count > reservation.iov_len)
        throw std::runtime_error("attempted to back up into reservations beyond the last");

    reservation.iov_len -= count;

    if(evbuffer_commit_space(m_buffer, &reservation, 1) < 0)
        throw std::runtime_error("evbuffer_commit_space has failed");

    m_position += reservation.iov_len;

    m_reservation.reset();
}

int64_t PBOutputOverLLBuffer::ByteCount() const {
    return m_position;
}
