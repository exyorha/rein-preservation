#include <Translator/ThreadContextAssociation.h>
#include <Translator/JITThreadContext.h>

class JITThreadLocalContextPtr {
public:
    explicit JITThreadLocalContextPtr() noexcept;
    ~JITThreadLocalContextPtr();

    JITThreadLocalContextPtr(const JITThreadLocalContextPtr &other) = delete;
    JITThreadLocalContextPtr &operator =(const JITThreadLocalContextPtr &other) noexcept = delete;

    JITThreadLocalContextPtr(const JITThreadContextPtr &other) noexcept;
    JITThreadLocalContextPtr &operator =(const JITThreadContextPtr &other) noexcept;

    inline JITThreadContext *get() const noexcept {
        return m_pointer;
    }

    inline JITThreadContext &operator *() const {
        return *m_pointer;
    }

    inline JITThreadContext *operator ->() const {
        return m_pointer;
    }

private:
    JITThreadContext *m_pointer;
};


JITThreadLocalContextPtr::JITThreadLocalContextPtr() noexcept : m_pointer(nullptr) {

}

JITThreadLocalContextPtr::~JITThreadLocalContextPtr() {
    if(m_pointer) {
        m_pointer->threadStateTeardown();
        m_pointer->release();
    }
}

JITThreadLocalContextPtr::JITThreadLocalContextPtr(const JITThreadContextPtr &other) noexcept : m_pointer(other.get()) {
    if(m_pointer) {
        m_pointer->addReference();
    }
}

JITThreadLocalContextPtr &JITThreadLocalContextPtr::operator =(const JITThreadContextPtr &other) noexcept {
    if(m_pointer != other.get()) {
        if(m_pointer) {
            m_pointer->threadStateTeardown();
            m_pointer->release();
        }

        m_pointer = other.get();

        if(m_pointer)
            m_pointer->addReference();
    }

    return *this;
}

static thread_local JITThreadLocalContextPtr ThreadJITAssociation;

JITThreadContext *getJITContextOfCurrentThread() noexcept {
    return ThreadJITAssociation.get();
}

void setJITContextOfCurrentThread(JITThreadContext *context) noexcept {
    JITThreadContextPtr pointer(context);
    if(context)
        context->addReference();

    ThreadJITAssociation = pointer;
}
