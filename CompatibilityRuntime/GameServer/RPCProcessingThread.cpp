#include "RPCProcessingThread.h"

RPCProcessingThread::RPCProcessingThread(std::unique_ptr<grpc::ServerCompletionQueue> &&queue) : m_queue(std::move(queue)),
    m_thread(&RPCProcessingThread::processingThread, this) {

}

RPCProcessingThread::~RPCProcessingThread() {
    m_queue->Shutdown();

    m_thread.join();
}

void RPCProcessingThread::processingThread() {
    void *tag;
    bool ok;

    while(m_queue->Next(&tag, &ok)) {
        printf("RPCProcessingThread::processingThread(%p, %d)\n", tag, ok);
    }
}
