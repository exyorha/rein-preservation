#ifndef GRPC_PB_INPUT_OVER_LL_BUFFER_H
#define GRPC_PB_INPUT_OVER_LL_BUFFER_H

#include <google/protobuf/io/zero_copy_stream.h>

struct evbuffer;

class PBInputOverLLBuffer final : public google::protobuf::io::ZeroCopyInputStream {
public:
    explicit PBInputOverLLBuffer(evbuffer *buffer);
    ~PBInputOverLLBuffer() override;

    PBInputOverLLBuffer(const PBInputOverLLBuffer &other) = delete;
    PBInputOverLLBuffer &operator =(const PBInputOverLLBuffer &other) = delete;

    bool Next(const void** data, int* size) override;
    void BackUp(int count) override;
    bool Skip(int count) override;
    int64_t ByteCount() const override;

private:
    evbuffer *m_buffer;
    size_t m_pos;
};

#endif
