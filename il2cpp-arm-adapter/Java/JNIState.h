#ifndef JNI_STATE_H
#define JNI_STATE_H

#include <type_traits>
#include <memory>
#include <cstdio>
#include <unordered_map>
#include <vector>

#include <Java/JNIThrowable.h>

class JNIState {
public:
    JNIState();
    ~JNIState();

    JNIState(const JNIState &other) = delete;
    JNIState &operator =(const JNIState &other) = delete;

    static JNIState &get();

    template<typename Func>
    auto guard(const Func &func) -> typename std::invoke_result<Func>::type {
        if(m_pendingException) {
            throw std::runtime_error("JNI error: calling JNI with an exception pending");
        }

        try {
            return func();
        } catch(const std::exception &e) {
            printf("JNI exception: %s\n", e.what());
            m_pendingException = std::make_shared<JNIThrowable>(e);
            return DefaultReturn<typename std::invoke_result<Func>::type>::returnValue();
        } catch(...) {
            printf("JNI exception: <no message available>\n");
            m_pendingException = std::make_shared<JNIThrowable>();
            return DefaultReturn<typename std::invoke_result<Func>::type>::returnValue();
        }
    }

    inline std::shared_ptr<JNIThrowable> exceptionOccurred() const {
        return m_pendingException;
    }

    template<typename T>
    intptr_t makeLocalReference(const std::shared_ptr<T> &object) {
        return makeLocalReferenceInternal(std::static_pointer_cast<JNIObject>(object));
    }

    inline void exceptionClear() {
        m_pendingException.reset();
    }

    template<typename T>
    std::shared_ptr<T> resolve(intptr_t reference) {
        auto object = resolveInternal(reference);
        auto casted = std::dynamic_pointer_cast<T>(object);
        if(!casted && object)
            throw std::runtime_error("the object is not of the expected type");

        return casted;
    }

    template<typename T>
    std::shared_ptr<T> resolveNonNull(intptr_t reference) {
        auto ref = resolve<T>(reference);

        if(!ref)
            throw std::runtime_error("unexpected null reference");

        return ref;
    }

    void deleteLocalRef(intptr_t ref);

    void pushLocalFrame();
    void popLocalFrame();

private:
    template<typename T>
    struct DefaultReturn {
        inline static T returnValue() {
            return {};
        }
    };

    intptr_t makeLocalReferenceInternal(const std::shared_ptr<JNIObject> &object);

    std::shared_ptr<JNIObject> resolveInternal(intptr_t reference);

    static thread_local std::unique_ptr<JNIState> m_state;

    using LocalFrame = std::unordered_map<intptr_t, std::shared_ptr<JNIObject>>;

    std::shared_ptr<JNIThrowable> m_pendingException;
    intptr_t m_lastLocalReference;
    std::vector<LocalFrame> m_localStack;
};

template<>
struct JNIState::DefaultReturn<void> {
    inline static void returnValue() {};
};


#endif
