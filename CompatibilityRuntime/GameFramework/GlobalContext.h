#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include <ELF/ElfLinkingSet.h>

#include <filesystem>

#include <Translator/ThunkManager.h>
#include <Translator/DiversionManager.h>
#include <Translator/JIT.h>

#include <ThunkSymbolSource.h>

class ICallImplementation;

class GlobalContext {
public:
    GlobalContext();
    ~GlobalContext();

    GlobalContext(const GlobalContext &other) = delete;
    GlobalContext &operator =(const GlobalContext &other) = delete;

    inline ThunkManager &thunkManager() {
        return m_thunkManager;
    }

    inline DiversionManager &diversionManager() {
        return m_diversionManager;
    }

    inline JIT &jit() {
        return m_jit;
    }

    inline const ElfLinkingSet &linkingSet() const {
        return m_linkingSet;
    }

    static const uint32_t PageSize;

    static inline GlobalContext &get() {
        return GlobalContextRegisterer::get();
    }

    static bool precompiledICallUseDisallowed;

    inline ICallImplementation &icallImplementation() {
        return *m_icallImplementation;
    }

private:
    class GlobalContextRegisterer {
    public:
        explicit GlobalContextRegisterer(GlobalContext *context);
        ~GlobalContextRegisterer();

        GlobalContextRegisterer(const GlobalContextRegisterer &other) = delete;
        GlobalContextRegisterer &operator =(const GlobalContextRegisterer &other) = delete;

        static inline GlobalContext &get() {
            auto context = m_context;
            if(!context)
                missingGlobalContext();

            return *context;
        }

    private:
        [[noreturn]] static void missingGlobalContext();

        static GlobalContext *m_context;
    };

    static std::filesystem::path thisLibraryDirectory();

    static uint32_t queryPageSize();

    GlobalContextRegisterer m_registerer;
    ThunkManager m_thunkManager;
    DiversionManager m_diversionManager;
    JIT m_jit;
    ThunkSymbolSource m_thunkSymbolSource;
    ElfLinkingSet m_linkingSet;
    std::unique_ptr<ICallImplementation> m_icallImplementation;
};

#endif
