#ifndef BIONIC_BIONIC_THREADING_H
#define BIONIC_BIONIC_THREADING_H

#include <chrono>
#include <cstring>
#include <cstdint>
#include <cerrno>
#include <condition_variable>
#include <shared_mutex>

class JITThreadContext;

/*
 * We largely replace Bionic threading implementation by our own to reduce
 * the porting burden. To do this, we carry our own, somewhat modified,
 * build of Bionic. As such, we can freely depend on the implementation
 * details of it.
 */

enum {
  BIONIC_TLS_SLOT_SELF = 0, // The kernel requires this specific slot for x86.
  BIONIC_TLS_SLOT_THREAD_ID,
  BIONIC_TLS_SLOT_ERRNO,

  // These two aren't used by bionic itself, but allow the graphics code to
  // access TLS directly rather than using the pthread API.
  BIONIC_TLS_SLOT_OPENGL_API = 3,
  BIONIC_TLS_SLOT_OPENGL = 4,

  // This slot is only used to pass information from the dynamic linker to
  // libc.so when the C library is loaded in to memory. The C runtime init
  // function will then clear it. Since its use is extremely temporary,
  // we reuse an existing location that isn't needed during libc startup.
  BIONIC_TLS_SLOT_BIONIC_PREINIT = BIONIC_TLS_SLOT_OPENGL_API,

  BIONIC_TLS_SLOT_STACK_GUARD = 5, // GCC requires this specific slot for x86.
  BIONIC_TLS_SLOT_DLERROR,

  BIONIC_TLS_SLOTS, // Must come last!

  /*
   * Specifc to us.
   */
  BIONIC_TLS_SLOT_KEY_DATA = BIONIC_TLS_SLOT_OPENGL,
  BIONIC_TLS_SLOT_CLEANUP_STACK = BIONIC_TLS_SLOT_OPENGL_API,

};

/*
 * The maximum that can be used: BIONIC_PTHREAD_KEY_RESERVED_COUNT (up to 13) + PTHREAD_KEYS_MAX (128)
 */
static constexpr size_t BIONIC_PTHREAD_KEY_COUNT = 13 + 128;

/*
 * This is our own custom type that's *laid over* bionic's pthread_mutex_t - that is,
 * the pointers to pthread_mutex_t are reinterpreted as pointers to our mutex structure.
 * The client application allocates pthread_mutexes, so we need to be strictly layout
 * compatible.
 */
class BaseMutexImplementation;
struct bionic_mutex_overlay {
    /*
     * We only store a pointer to the real mutex implementation in the
     * mutex overlay, since the platform mutex can be of any size and
     * may have any alignment requirements. As such, we need to create
     * and allocate the actual mutex object.
     *
     * However, the application is not guaranteed to call pthread_mutex_init.
     * Instead, the mutex can be statically initialized to one of the
     * specified values, and is supposed to work like that.
     *
     * We use our knowledge of bionic's mutex layout and static initializers
     * in order to determine if the mutex is currently initialized or not,
     * and if it is not, to what type do we need to initialize it.
     */
    volatile uint32_t initializationStateWord;
    BaseMutexImplementation *implementation;

    /*
     * Not initialized. If used, should be initialized to the specified type.
     */
    static constexpr uint32_t UninitializedNormal = 0 << 14;
    static constexpr uint32_t UninitializedRecursive = 1 << 14;
    static constexpr uint32_t UninitializedErrorCheck = 2 << 14;

    /*
     * Initialized and has a valid BaseMutexImplementation.
     */
    static constexpr uint32_t Initialized = 0xFAAA3040;

    /*
     * Destroyed.
     */
    static constexpr uint32_t Destroyed = 0x55555555;
} __attribute__((packed,aligned(4)));

static_assert(sizeof(bionic_mutex_overlay) <= 40 && alignof(bionic_mutex_overlay) <= 4, "bionic_mutex_overlay must be layout-compatible with pthread_mutex_t");

using bionic_mutex_attr = int64_t;

struct bionic_cond_overlay {
    /*
     * Condition variables use a similar system of point-of-use initialization.
     */
    volatile uint32_t initializationStateWord;
    std::condition_variable_any *implementation;

    /*
     * Not initialized.
     */
    static constexpr uint32_t Uninitialized = 0;

    /*
     * Initialized.
     */
    static constexpr uint32_t Initialized = 0x6c278b8f;

    /*
     * Destroyed.
     */
    static constexpr uint32_t Destroyed = 0x55555555;
} __attribute__((packed,aligned(4)));

static_assert(sizeof(bionic_cond_overlay) <= 48 && alignof(bionic_cond_overlay) <= 4, "bionic_cond_overlay must be layout-compatible with pthread_cond_t");

using bionic_cond_attr = int64_t;

class ReadWriteLockImplementation;
struct bionic_rwlock_overlay {
    /*
     * R/W locks use a similar system of point-of-use initialization.
     */
    volatile uint32_t initializationStateWord;
    ReadWriteLockImplementation *implementation;

    /*
     * Not initialized.
     */
    static constexpr uint32_t Uninitialized = 0;

    /*
     * Initialized.
     */
    static constexpr uint32_t Initialized = 0x10f5d968;

    /*
     * Destroyed.
     */
    static constexpr uint32_t Destroyed = 0x55555555;
} __attribute__((packed,aligned(4)));

static_assert(sizeof(bionic_rwlock_overlay) <= 56 && alignof(bionic_rwlock_overlay) <= 4, "bionic_rwlock_overlay must be layout-compatible with pthread_rwlock_t");

using bionic_rwlock_attr = int64_t;

/*
 * It's an opaque pointer in bionic.
 */
using bionic_pthread = JITThreadContext *;

struct bionic_pthread_attr {
  uint32_t flags;
  void* stack_base;
  size_t stack_size;
  size_t guard_size;
  int32_t sched_policy;
  int32_t sched_priority;
  char __reserved[16];
};

int plat_pthread_mutex_init(bionic_mutex_overlay *mutex, const bionic_mutex_attr *attr);
int plat_pthread_mutex_destroy(bionic_mutex_overlay *mutex);
int plat_pthread_mutex_lock(bionic_mutex_overlay *mutex);
int plat_pthread_mutex_unlock(bionic_mutex_overlay *mutex);
int plat_pthread_mutex_timedlock(bionic_mutex_overlay *mutex, const struct timespec *timeout);
int plat_pthread_mutex_trylock(bionic_mutex_overlay *mutex);

int plat_pthread_cond_broadcast(bionic_cond_overlay *cond);
int plat_pthread_cond_destroy(bionic_cond_overlay *cond);
int plat_pthread_cond_init(bionic_cond_overlay *cond, const bionic_cond_attr *condattr);
int plat_pthread_cond_signal(bionic_cond_overlay *cond);
int plat_pthread_cond_timedwait(bionic_cond_overlay *cond, bionic_mutex_overlay *mutex, const struct timespec *timeout);
int plat_pthread_cond_wait(bionic_cond_overlay *cond, bionic_mutex_overlay *mutex);

int plat_pthread_rwlock_destroy(bionic_rwlock_overlay *rwlock);
int plat_pthread_rwlock_init(bionic_rwlock_overlay *rwlock, const bionic_rwlock_attr *attr);
int plat_pthread_rwlock_rdlock(bionic_rwlock_overlay *rwlock);
int plat_pthread_rwlock_timedrdlock(bionic_rwlock_overlay *rwlock, const struct timespec *timeout);
int plat_pthread_rwlock_timedwrlock(bionic_rwlock_overlay *rwlock, const struct timespec *timeout);
int plat_pthread_rwlock_tryrdlock(bionic_rwlock_overlay *rwlock);
int plat_pthread_rwlock_trywrlock(bionic_rwlock_overlay *rwlock);
int plat_pthread_rwlock_unlock(bionic_rwlock_overlay *rwlock);
int plat_pthread_rwlock_wrlock(bionic_rwlock_overlay *rwlock);

int plat_pthread_create(bionic_pthread *thread, const bionic_pthread_attr *attr, void *(*func)(void *arg), void *arg);
int plat_pthread_getattr_np(bionic_pthread thread, bionic_pthread_attr *attr);
int plat_pthread_detach(bionic_pthread thread);
int plat_pthread_join(bionic_pthread thread, void **result);
[[noreturn]] void plat_pthread_exit(void *result);
int plat_pthread_setname_np(bionic_pthread thread, const char*name);
int plat_pthread_kill(bionic_pthread thread, int signal);

class BaseMutexImplementation {
protected:
    BaseMutexImplementation() = default;

public:
    virtual ~BaseMutexImplementation() = default;
    BaseMutexImplementation(const BaseMutexImplementation &other) = delete;
    BaseMutexImplementation &operator =(const BaseMutexImplementation &other) = delete;

    static BaseMutexImplementation *createImplementationForAttributes(const bionic_mutex_attr *attr);
    static BaseMutexImplementation *createImplementationForStaticallyInitializedValue(uint32_t value);

    virtual int lock() noexcept = 0;
    virtual int trylock() noexcept = 0;
    virtual int unlock() noexcept = 0;

    virtual void conditionVariableWait(std::condition_variable_any &condvar) = 0;
    virtual std::cv_status conditionVariableWaitUntil(std::condition_variable_any &condvar, const std::chrono::steady_clock::time_point &duration) = 0;
};

template<typename T>
struct MutexToConditionVariableMutexConverter {
public:
    static inline std::mutex *convert(T &type) {
        return nullptr;
    }
};


template<>
struct MutexToConditionVariableMutexConverter<std::mutex> {
public:
    static inline std::mutex *convert(std::mutex &type) {
        return &type;
    }
};


template<typename Mutex>
class MutexImplementation final : public BaseMutexImplementation {
public:
    MutexImplementation() = default;
    ~MutexImplementation() override = default;

    int lock() noexcept override {
        m_mutex.lock();
        return 0;
    }

    int trylock() noexcept override {
        if(!m_mutex.try_lock())
            return EBUSY;

        return 0;
    }

    int unlock() noexcept override {
        m_mutex.unlock();
        return 0;
    }

    void conditionVariableWait(std::condition_variable_any &condvar) override {
        std::unique_lock<Mutex> locker(m_mutex, std::adopt_lock);

        condvar.wait(locker);

        locker.release();
    }

    std::cv_status conditionVariableWaitUntil(std::condition_variable_any &condvar, const std::chrono::steady_clock::time_point &duration) override {
        std::unique_lock<Mutex> locker(m_mutex, std::adopt_lock);

        auto result = condvar.wait_until<std::unique_lock<Mutex>, std::chrono::steady_clock>(locker, duration);

        locker.release();

        return result;
    }

private:
    Mutex m_mutex;
};

class ReadWriteLockImplementation {
public:
    ReadWriteLockImplementation() : m_exclusivelyLocked(false) {

    }

    ~ReadWriteLockImplementation() = default;

    ReadWriteLockImplementation(const ReadWriteLockImplementation &other) = delete;
    ReadWriteLockImplementation &operator =(const ReadWriteLockImplementation &other) = delete;

    inline void sharedLock() noexcept {
        m_mutex.lock();
    }

    inline bool timedSharedLock(const std::chrono::system_clock::duration &timeout) noexcept {
        return m_mutex.try_lock_shared_for(timeout);
    }

    inline bool trySharedLock() noexcept {
        return m_mutex.try_lock_shared();
    }

    inline void uniqueLock() noexcept {
        m_mutex.lock();
        m_exclusivelyLocked = true;
    }

    inline bool timedUniqueLock(const std::chrono::system_clock::duration &timeout) noexcept {
        if(m_mutex.try_lock_for(timeout)) {
            m_exclusivelyLocked = true;

            return true;
        } else {
            return false;
        }
    }

    inline bool tryUniqueLock() noexcept {
        if(m_mutex.try_lock()) {
            m_exclusivelyLocked = true;

            return true;
        } else {
            return false;
        }
    }

    inline void unlock() noexcept {
        if(m_exclusivelyLocked) {
            m_exclusivelyLocked = false;

            m_mutex.unlock();
        } else {
            m_mutex.unlock_shared();
        }
    }

private:
    std::shared_timed_mutex m_mutex;
    bool m_exclusivelyLocked;
};

#endif
