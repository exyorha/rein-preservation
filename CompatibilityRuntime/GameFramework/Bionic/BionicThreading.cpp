#include <Bionic/BionicThreading.h>

#include <Translator/WorldStoppingScope.h>

#include <condition_variable>
#include <errno.h>
#include <sys/time.h>

#include <mutex>

#include "Translator/JITThreadContext.h"
#include "support.h"

int plat_pthread_mutex_init(bionic_mutex_overlay *mutex, const bionic_mutex_attr *attr) {
    /*
     * The application sometimes initializes already initialized mutexes, and we need to be tolerant of that.
     */
    auto oldValue = __atomic_exchange_n(&mutex->initializationStateWord, bionic_mutex_overlay::Initialized, __ATOMIC_SEQ_CST);
    if(oldValue != bionic_mutex_overlay::Initialized) {
        try {
            mutex->implementation = BaseMutexImplementation::createImplementationForAttributes(attr);
        } catch(...) {
            return ENOMEM;
        }
    }

    return 0;
}

int plat_pthread_mutex_destroy(bionic_mutex_overlay *mutex) {
    auto oldValue = __atomic_exchange_n(&mutex->initializationStateWord, bionic_mutex_overlay::Destroyed, __ATOMIC_SEQ_CST);
    if(oldValue == bionic_mutex_overlay::Initialized) {
        delete mutex->implementation;
        mutex->implementation = nullptr;
    }

    return 0;
}

static int plat_mutex_point_of_use_init(bionic_mutex_overlay *mutex) {
    auto status = __atomic_load_n(&mutex->initializationStateWord, __ATOMIC_CONSUME);
    if(status != bionic_mutex_overlay::Initialized) {
        /*
         * The mutex requires a late initialization, because it was statically constructed.
         */
        WorldStoppingScope guardedScope;

        /*
         * No other ARM thread can run while we are in this scope.
         */

        status = __atomic_load_n(&mutex->initializationStateWord, __ATOMIC_CONSUME);
        if(status != bionic_mutex_overlay::Initialized) {

            /*
             * Still requires initialization.
             */

            if(status == bionic_mutex_overlay::Destroyed) {
                panic("plat_pthread_point_of_use_init: attempted to use a destroyed mutex\n");
            }

            try {
                mutex->implementation = BaseMutexImplementation::createImplementationForStaticallyInitializedValue(status);
            } catch(...) {
                return ENOMEM;
            }

            __atomic_store_n(&mutex->initializationStateWord, bionic_mutex_overlay::Initialized, __ATOMIC_SEQ_CST);
        }
    }

    return 0;
}

int plat_pthread_mutex_lock(bionic_mutex_overlay *mutex) {
    auto result = plat_mutex_point_of_use_init(mutex);
    if(result != 0)
        return result;

    return mutex->implementation->lock();
}

int plat_pthread_mutex_unlock(bionic_mutex_overlay *mutex) {
    auto result = plat_mutex_point_of_use_init(mutex);
    if(result != 0)
        return result;

    return mutex->implementation->unlock();
}

int plat_pthread_mutex_timedlock(bionic_mutex_overlay *mutex, const struct timespec *timeout) {
    (void)timeout;
    /*
     * We don't actually implement timed locking, because std::timed_mutex is not compatible with std::condition_variable.
     */
    auto result = plat_mutex_point_of_use_init(mutex);
    if(result != 0)
        return result;

    return mutex->implementation->lock();
}

int plat_pthread_mutex_trylock(bionic_mutex_overlay *mutex) {
    auto result = plat_mutex_point_of_use_init(mutex);
    if(result != 0)
        return result;

    return mutex->implementation->trylock();
}

BaseMutexImplementation *BaseMutexImplementation::createImplementationForAttributes(const bionic_mutex_attr *attr) {
    if(attr == nullptr)
        return createImplementationForStaticallyInitializedValue(bionic_mutex_overlay::UninitializedNormal);
    else {
        return createImplementationForStaticallyInitializedValue((*attr & 3) << 14);
    }
}

BaseMutexImplementation *BaseMutexImplementation::createImplementationForStaticallyInitializedValue(uint32_t value) {
    if(value == bionic_mutex_overlay::UninitializedRecursive)
        return new MutexImplementation<std::recursive_mutex>();
    else
        return new MutexImplementation<std::mutex>();
}

static std::chrono::system_clock::duration convertLockTimeout(const struct timespec *timeout) noexcept {
    return std::chrono::seconds(timeout->tv_sec) + std::chrono::nanoseconds(timeout->tv_nsec);
}

int plat_pthread_cond_init(bionic_cond_overlay *cond, const bionic_cond_attr *condattr) {
    try {
        cond->implementation = new std::condition_variable_any();
    } catch(...) {
        return ENOMEM;
    }

    auto oldValue = __atomic_exchange_n(&cond->initializationStateWord, bionic_cond_overlay::Initialized, __ATOMIC_SEQ_CST);
    if(oldValue == bionic_cond_overlay::Initialized)
        panic("plat_pthread_cond_init: reinitialized an already initialized condition variable");

    return 0;
}

int plat_pthread_cond_destroy(bionic_cond_overlay *cond) {
    auto oldValue = __atomic_exchange_n(&cond->initializationStateWord, bionic_cond_overlay::Destroyed, __ATOMIC_SEQ_CST);
    if(oldValue == bionic_cond_overlay::Initialized) {
        delete cond->implementation;
        cond->implementation = nullptr;
    }

    return 0;
}

static int plat_cond_point_of_use_init(bionic_cond_overlay *cond) {
    auto status = __atomic_load_n(&cond->initializationStateWord, __ATOMIC_CONSUME);
    if(status != bionic_cond_overlay::Initialized) {
        /*
         * The condition variable requires a late initialization, because it was statically constructed.
         */
        WorldStoppingScope guardedScope;

        /*
         * No other ARM thread can run while we are in this scope.
         */

        status = __atomic_load_n(&cond->initializationStateWord, __ATOMIC_CONSUME);
        if(status != bionic_cond_overlay::Initialized) {
            /*
             * Still requires initialization.
             */

            if(status == bionic_cond_overlay::Destroyed) {
                panic("plat_cond_point_of_use_init: attempted to use a destroyed condition variable\n");
            }

            try {
                cond->implementation = new std::condition_variable_any();
            } catch(...) {
                return ENOMEM;
            }

            __atomic_store_n(&cond->initializationStateWord, bionic_cond_overlay::Initialized, __ATOMIC_SEQ_CST);
        }
    }

    return 0;
}

int plat_pthread_cond_broadcast(bionic_cond_overlay *cond) {
    auto result = plat_cond_point_of_use_init(cond);
    if(result != 0)
        return result;

    cond->implementation->notify_all();

    return 0;
}

int plat_pthread_cond_signal(bionic_cond_overlay *cond) {
    auto result = plat_cond_point_of_use_init(cond);
    if(result != 0)
        return result;

    cond->implementation->notify_one();

    return 0;
}

static int plat_pthread_cond_setup(bionic_cond_overlay *cond, bionic_mutex_overlay *mutex) {

    auto result = plat_cond_point_of_use_init(cond);
    if(result != 0)
        return result;

    result = plat_mutex_point_of_use_init(mutex);
    if(result != 0)
        return result;

    return 0;
}

int plat_pthread_cond_timedwait(bionic_cond_overlay *cond, bionic_mutex_overlay *mutex, const struct timespec *timeout) {
    auto result = plat_pthread_cond_setup(cond, mutex);
    if(result != 0)
        return result;

    auto condResult = mutex->implementation->conditionVariableWaitUntil(*cond->implementation,
                                                                      std::chrono::steady_clock::time_point(convertLockTimeout(timeout)));

    if(condResult == std::cv_status::no_timeout)
        return 0;
    else
        return ETIMEDOUT;
}

int plat_pthread_cond_wait(bionic_cond_overlay *cond, bionic_mutex_overlay *mutex) {
    auto result = plat_pthread_cond_setup(cond, mutex);
    if(result != 0)
        return result;

    mutex->implementation->conditionVariableWait(*cond->implementation);

    return 0;
}

int plat_pthread_rwlock_init(bionic_rwlock_overlay *rwlock, const bionic_rwlock_attr *attr) {
    try {
        rwlock->implementation = new ReadWriteLockImplementation;
    } catch(...) {
        return ENOMEM;
    }

    auto oldValue = __atomic_exchange_n(&rwlock->initializationStateWord, bionic_rwlock_overlay::Initialized, __ATOMIC_SEQ_CST);
    if(oldValue == bionic_rwlock_overlay::Initialized)
        panic("plat_pthread_rwlock_init: reinitialized an already initialized read/write lock");

    return 0;
}

int plat_pthread_rwlock_destroy(bionic_rwlock_overlay *rwlock) {
    auto oldValue = __atomic_exchange_n(&rwlock->initializationStateWord, bionic_rwlock_overlay::Destroyed, __ATOMIC_SEQ_CST);
    if(oldValue == bionic_rwlock_overlay::Initialized) {
        delete rwlock->implementation;
        rwlock->implementation = nullptr;
    }

    return 0;
}

static int plat_rwlock_point_of_use_init(bionic_rwlock_overlay *rwlock) {
    auto status = __atomic_load_n(&rwlock->initializationStateWord, __ATOMIC_CONSUME);
    if(status != bionic_rwlock_overlay::Initialized) {
        /*
         * The R/W lock requires a late initialization, because it was statically constructed.
         */
        WorldStoppingScope guardedScope;

        /*
         * No other ARM thread can run while we are in this scope.
         */

        status = __atomic_load_n(&rwlock->initializationStateWord, __ATOMIC_CONSUME);
        if(status != bionic_rwlock_overlay::Initialized) {
            /*
             * Still requires initialization.
             */

            if(status == bionic_rwlock_overlay::Destroyed) {
                panic("plat_rwlock_point_of_use_init: attempted to use a destroyed R/W lock\n");
            }

            try {
                rwlock->implementation = new ReadWriteLockImplementation;
            } catch(...) {
                return ENOMEM;
            }

            __atomic_store_n(&rwlock->initializationStateWord, bionic_rwlock_overlay::Initialized, __ATOMIC_SEQ_CST);
        }
    }

    return 0;
}

int plat_pthread_rwlock_rdlock(bionic_rwlock_overlay *rwlock) {
    auto status = plat_rwlock_point_of_use_init(rwlock);

    if(status != 0)
        return status;

    rwlock->implementation->sharedLock();

    return 0;
}

int plat_pthread_rwlock_timedrdlock(bionic_rwlock_overlay *rwlock, const struct timespec *timeout) {
    auto status = plat_rwlock_point_of_use_init(rwlock);

    if(status != 0)
        return status;

    if(rwlock->implementation->timedSharedLock(convertLockTimeout(timeout)))
        return 0;
    else
        return ETIMEDOUT;
}

int plat_pthread_rwlock_timedwrlock(bionic_rwlock_overlay *rwlock, const struct timespec *timeout) {
    auto status = plat_rwlock_point_of_use_init(rwlock);

    if(status != 0)
        return status;

    if(rwlock->implementation->timedUniqueLock(convertLockTimeout(timeout)))
        return 0;
    else
        return ETIMEDOUT;
}

int plat_pthread_rwlock_tryrdlock(bionic_rwlock_overlay *rwlock) {
    auto status = plat_rwlock_point_of_use_init(rwlock);

    if(status != 0)
        return status;

    if(rwlock->implementation->trySharedLock())
        return 0;
    else
        return EBUSY;
}

int plat_pthread_rwlock_trywrlock(bionic_rwlock_overlay *rwlock) {
    auto status = plat_rwlock_point_of_use_init(rwlock);

    if(status != 0)
        return status;

    if(rwlock->implementation->tryUniqueLock())
        return 0;
    else
        return EBUSY;
}

int plat_pthread_rwlock_unlock(bionic_rwlock_overlay *rwlock) {
    auto status = plat_rwlock_point_of_use_init(rwlock);

    if(status != 0)
        return status;

    rwlock->implementation->unlock();

    return 0;
}

int plat_pthread_rwlock_wrlock(bionic_rwlock_overlay *rwlock) {
    auto status = plat_rwlock_point_of_use_init(rwlock);

    if(status != 0)
        return status;

    rwlock->implementation->uniqueLock();

    return 0;
}

int plat_pthread_create(bionic_pthread *thread, const bionic_pthread_attr *attr, void *(*func)(void *arg), void *arg) {
    try {
        JITThreadContextPtr newThreadContext;

        if(attr) {
            if(attr->stack_base && attr->stack_size) {
                newThreadContext = JITThreadContextPtr(new JITThreadContext(attr->stack_base, attr->stack_size));
            } else {
                newThreadContext = JITThreadContextPtr(new JITThreadContext());
            }
        }

        newThreadContext->startNewHostThread(func, arg);

        if(attr && (attr->flags & 1)) {
            newThreadContext->detach();
        }

        *thread = newThreadContext.get();

        return 0;
    } catch(const std::exception &e) {
        fprintf(stderr, "plat_pthread_create has failed: %s\n", e.what());
        return EAGAIN;
    }
}

int plat_pthread_getattr_np(bionic_pthread thread, bionic_pthread_attr *attr) {
    memset(attr, 0, sizeof(*attr));

    attr->stack_base = thread->threadARMStack();
    attr->stack_size = thread->threadARMStackSize();

    return 0;
}

int plat_pthread_detach(bionic_pthread thread) {
    try {
        thread->detach();

        return 0;
    } catch(const std::exception &e) {
        fprintf(stderr, "plat_pthread_detach has failed: %s\n", e.what());

        return EINVAL;
    }
}

int plat_pthread_join(bionic_pthread thread, void **result) {
    try {
        thread->join(result);

        return 0;
    } catch(const std::exception &e) {
        fprintf(stderr, "plat_pthread_join has failed: %s\n", e.what());

        return EINVAL;
    }
}

void plat_pthread_exit(void *result) {
    JITThreadContext::get().exitHostThread(result);
}

int plat_pthread_kill(bionic_pthread thread, int signal) {
    if(&JITThreadContext::get() != thread)
        panic("plat_pthread_kill: can only kill self\n");

    plat_pthread_exit(reinterpret_cast<void *>(-static_cast<intptr_t>(signal)));
}

int plat_pthread_setname_np(bionic_pthread thread, const char*name) {
    if(thread == &JITThreadContext::get()) {
        pthread_setname_np(pthread_self(), name);
    }
    return 0;
}
