#ifndef GRPC_PB_OUTPUT_OVER_LL_BUFFER_H
#define GRPC_PB_OUTPUT_OVER_LL_BUFFER_H

#include <google/protobuf/io/zero_copy_stream.h>

#include <event2/buffer.h>

#include <optional>

struct evbuffer;

class PBOutputOverLLBuffer final : public google::protobuf::io::ZeroCopyOutputStream {
public:
    explicit PBOutputOverLLBuffer(evbuffer *buffer);
    ~PBOutputOverLLBuffer() override;

    PBOutputOverLLBuffer(const PBOutputOverLLBuffer &other) = delete;
    PBOutputOverLLBuffer &operator =(const PBOutputOverLLBuffer &other) = delete;

    bool Next(void** data, int* size) override;
    void BackUp(int count) override;
    int64_t ByteCount() const override;

private:
    evbuffer *m_buffer;
    std::optional<evbuffer_iovec> m_reservation;
    size_t m_position;
};

#endif

