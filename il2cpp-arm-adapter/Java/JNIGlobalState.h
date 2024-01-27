#ifndef JNI_GLOBAL_STATE_H
#define JNI_GLOBAL_STATE_H

#include <memory>
#include <cinttypes>
#include <optional>
#include <mutex>
#include <unordered_map>

class JNIObject;
class JNIClass;

class JNIGlobalState {
public:
    JNIGlobalState();
    ~JNIGlobalState();

    void init();

    static JNIGlobalState &get();

    intptr_t newGlobalRef(const std::shared_ptr<JNIObject> &object);
    void deleteGlobalRef(intptr_t ref);

    std::shared_ptr<JNIObject> resolveGlobalReference(intptr_t ref);

    std::shared_ptr<JNIClass> findClass(const std::string_view &name);

    static bool isGlobalReference(intptr_t ref) {
        return ref >= FirstGlobalReference;
    }

private:
    void registerClass(const std::shared_ptr<JNIClass> &classObject);

    template<typename T>
    inline void registerClass() {
        registerClass(T::makeClass());
    }

    static std::recursive_mutex m_globalStateMutex;
    static std::optional<JNIGlobalState> m_globalState;

    static constexpr intptr_t FirstGlobalReference = static_cast<uint64_t>(1U) << 62;

    std::unordered_map<std::string_view, std::shared_ptr<JNIClass>> m_classes;
    std::mutex m_globalReferenceMutex;
    std::unordered_map<intptr_t, std::shared_ptr<JNIObject>> m_globalReferences;
    intptr_t m_lastGlobalReference;
};


#endif

