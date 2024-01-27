#include <Java/JNIGlobalState.h>
#include <Java/JNIClass.h>

std::recursive_mutex JNIGlobalState::m_globalStateMutex;
std::optional<JNIGlobalState> JNIGlobalState::m_globalState;

JNIGlobalState::JNIGlobalState() : m_lastGlobalReference(FirstGlobalReference) {

}

JNIGlobalState::~JNIGlobalState() = default;

void JNIGlobalState::registerClass(const std::shared_ptr<JNIClass> &classObject) {
    m_classes.emplace(classObject->name(), classObject);
}

JNIGlobalState &JNIGlobalState::get() {
    {
        std::unique_lock<std::recursive_mutex> sharedLock(m_globalStateMutex);

        if(!m_globalState.has_value()) {
            m_globalState.emplace();
            m_globalState->init();
        }

        return *m_globalState;
    }
}

std::shared_ptr<JNIClass> JNIGlobalState::findClass(const std::string_view &name) {
    auto it = m_classes.find(name);
    if(it == m_classes.end())
        throw std::runtime_error("the class was not found: " + std::string(name));

    return it->second;
}

intptr_t JNIGlobalState::newGlobalRef(const std::shared_ptr<JNIObject> &object) {
    if(!object)
        return 0;

    std::unique_lock<std::mutex> locker(m_globalReferenceMutex);

    auto result = m_globalReferences.emplace(++m_lastGlobalReference, object);

    return result.first->first;
}

void JNIGlobalState::deleteGlobalRef(intptr_t ref) {
    size_t deleted;

    {
        std::unique_lock<std::mutex> locker(m_globalReferenceMutex);

        deleted = m_globalReferences.erase(ref);
    }

    if(deleted == 0)
        throw std::runtime_error("bad JNI global reference: " + std::to_string(ref));
}

std::shared_ptr<JNIObject> JNIGlobalState::resolveGlobalReference(intptr_t ref) {

    std::unique_lock<std::mutex> locker(m_globalReferenceMutex);

    auto it = m_globalReferences.find(ref);
    if(it == m_globalReferences.end())
        return {};

    return it->second;
}

