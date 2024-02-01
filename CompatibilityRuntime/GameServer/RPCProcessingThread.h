#ifndef RPC_PROCESSING_THREAD_H
#define RPC_PROCESSING_THREAD_H

#include <grpcpp/completion_queue.h>

#include <thread>

class RPCProcessingThread {
public:
    explicit RPCProcessingThread(std::unique_ptr<grpc::ServerCompletionQueue> &&queue);
    ~RPCProcessingThread();

    RPCProcessingThread(const RPCProcessingThread &other) = delete;
    RPCProcessingThread &operator =(const RPCProcessingThread &other) = delete;

private:
    void processingThread();

    std::unique_ptr<grpc::ServerCompletionQueue> m_queue;
    std::thread m_thread;
};

#endif

