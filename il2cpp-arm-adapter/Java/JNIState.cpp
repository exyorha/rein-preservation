#include <Java/JNIState.h>
#include <Java/JNIGlobalState.h>

thread_local std::unique_ptr<JNIState> JNIState::m_state;

JNIState::JNIState() : m_lastLocalReference(0) {
    m_localStack.emplace_back();
}

JNIState::~JNIState() = default;

void JNIState::pushLocalFrame() {
    m_localStack.emplace_back();
}

void JNIState::popLocalFrame() {
    if(m_localStack.size() <= 1)
        throw std::runtime_error("cannot pop the root frame");

    m_localStack.pop_back();
}

JNIState &JNIState::get() {
    auto &thisContext = m_state;

    if(!thisContext) {
        thisContext = std::make_unique<JNIState>();
    }

    return *thisContext;
}

intptr_t JNIState::makeLocalReferenceInternal(const std::shared_ptr<JNIObject> &object) {
    if(!object)
        return 0;

    auto &frame = m_localStack.back();
    auto result = frame.emplace(++m_lastLocalReference, object);

    return result.first->first;
}

void JNIState::deleteLocalRef(intptr_t ref) {
    for(auto &frame: m_localStack) {
        if(frame.erase(ref))
            return;
    }

    throw std::runtime_error("JNI Error: deleteLocalRef with invalid reference " + std::to_string(ref));
}

std::shared_ptr<JNIObject> JNIState::resolveInternal(intptr_t ref) {
    if(ref == 0)
        return nullptr;

    if(JNIGlobalState::isGlobalReference(ref)) {
        auto result = JNIGlobalState::get().resolveGlobalReference(ref);

        if(result)
            return result;
    } else {

        for(auto &frame: m_localStack) {
            auto it = frame.find(ref);
            if(it != frame.end()) {
                return it->second;
            }
        }
    }

    throw std::runtime_error("JNI Error: unable to resolve the reference " + std::to_string(ref));
}
