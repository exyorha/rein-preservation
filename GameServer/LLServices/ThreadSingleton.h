#ifndef LLSERVICES_THREAD_SINGLETON_H
#define LLSERVICES_THREAD_SINGLETON_H

#include <memory>

namespace LLServices {
    template<typename Type>
    class ThreadSingletonScope;

    template<typename Type>
    class ThreadSingleton {
    protected:
        ThreadSingleton() = default;
        ~ThreadSingleton() = default;

    public:
        ThreadSingleton(const ThreadSingleton &other) = delete;
        ThreadSingleton &operator =(const ThreadSingleton &other) = delete;

        static std::shared_ptr<Type> get() {
            auto instance = getNullable();

            if(!instance)
                throw std::logic_error("singleton is not initialized");

            return instance;
        }

        static inline std::shared_ptr<Type> getNullable() {
            return m_instance;
        }

    private:
        friend class ThreadSingletonScope<Type>;
        static thread_local std::shared_ptr<Type> m_instance;
    };


    template<typename Type>
    class ThreadSingletonScope {
    public:
        explicit ThreadSingletonScope(const std::shared_ptr<Type> &instance) : m_thisInstance(instance) {
            m_previousInstance = ThreadSingleton<Type>::m_instance;
            ThreadSingleton<Type>::m_instance = m_thisInstance;
        }

        ~ThreadSingletonScope() {
            if(ThreadSingleton<Type>::m_instance != m_thisInstance) // mismatched ThreadSingletonScope
                std::terminate();

            ThreadSingleton<Type>::m_instance = m_previousInstance;
        }

        ThreadSingletonScope(const ThreadSingletonScope &other) = delete;
        ThreadSingletonScope &operator =(const ThreadSingletonScope &other) = delete;

    private:
        std::shared_ptr<Type> m_previousInstance;
        std::shared_ptr<Type> m_thisInstance;
    };
}

#endif
