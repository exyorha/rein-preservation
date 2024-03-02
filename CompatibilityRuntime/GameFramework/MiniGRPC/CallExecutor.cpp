#include "CallExecutor.h"
#include "MiniGRPC/ExecutingCall.h"

namespace minigrpc {

    CallExecutor::CallExecutor() = default;

    CallExecutor::~CallExecutor() = default;

    void CallExecutor::add(std::unique_ptr<ExecutingCall> &&call) {
        std::unique_lock<std::mutex> locker(m_queueMutex);

        m_calls.emplace_back(std::move(call));
    }

    void CallExecutor::update() {
        std::list<std::unique_ptr<ExecutingCall>> disposal;

        {
            std::unique_lock<std::mutex> locker(m_queueMutex);

            for(auto it = m_calls.begin(); it != m_calls.end(); ) {
                if((*it)->update()) {
                    auto nextIt = it;
                    ++nextIt;

                    disposal.splice(disposal.end(), m_calls, it);

                    it = nextIt;
                } else {
                    ++it;
                }
            }
        }

        /*
         * All the completed calls on the disposal list will be deleted now,
         * with the mutex unlocked.
         */
    }
}
